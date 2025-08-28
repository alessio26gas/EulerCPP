/* -------------------------------------------------------------------------- *\
     ___ _   _ _    ___ ___  ___ ___ ___
    | __| | | | |  | __| _ \/ __| _ \ _ \
    | _|| |_| | |__| _||   / (__|  _/  _/
    |___|\___/|____|___|_|_\\___|_| |_|

    Copyright 2025 Alessio Improta

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
\* -------------------------------------------------------------------------- */
/**
 * @file initialization.cpp
 * @brief Implements initialization routines for EulerCPP simulations.
 *
 * This file provides the implementation of functions that initialize
 * simulation fields, numerical schemes, axisymmetry, and output configuration.
 * It also handles loading initial conditions from restart files.
 *
 * Functions:
 * - set_initial_conditions: loads either restart or default initial conditions
 * - initialize_simulation: sets up limiters, reconstruction, physics and output
 *
 * Supports parallel initialization using OpenMP for faster setup.
 *
 * @author Alessio Improta
 */

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <omp.h>

#include <eulercpp/simulation/initialization.hpp>
#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/math/gradients.hpp>
#include <eulercpp/math/reconstruction.hpp>
#include <eulercpp/math/solution_update.hpp>
#include <eulercpp/math/vectors.hpp>
#include <eulercpp/physics/axisymmetric.hpp>
#include <eulercpp/physics/riemann.hpp>
#include <eulercpp/output/logger.hpp>
#include <eulercpp/output/writer.hpp>

namespace eulercpp {

/**
 * @brief Set the initial conditions of the simulation.
 *
 * This function initializes the conservative variables in the simulation
 * either from a restart file or from default input values. It supports:
 * - Parallel initialization using OpenMP
 * - Block-specific initial conditions
 * - Error handling for mismatched restart files
 *
 * @param sim Reference to the Simulation object to initialize.
 * @throws std::runtime_error if the restart file cannot be read or
 * contains errors.
 */
void set_initial_conditions(Simulation& sim) {
    Input& input = sim.input;
    Mesh& mesh = sim.mesh;
    Fields& fields = sim.fields;

    sim.status.cfl = input.numerical.CFL;

    if (input.init.restart == 1) {
        Logger::info() << "Loading restart file " << input.init.restart_file;

        std::ifstream file(input.init.restart_file, std::ios::binary);
        if (!file) {
            throw std::runtime_error(
                "Unable to open restart file " + input.init.restart_file
            );
        }

        std::string line;
        bool found_restart = false;

        while (std::getline(file, line)) {
            if (line.compare(0, 23, "# EULERCPP Restart File") == 0) {
                found_restart = true;
                Logger::debug() << "Detected ASCII restart file.";

                int iter;
                double time;
                int elm_check, var_check;

                file >> iter >> time >> elm_check >> var_check;

                if (!file) {
                    throw std::runtime_error(
                        "Error reading restart file header values."
                    );
                }
                if (elm_check != mesh.n_elements) {
                    throw std::runtime_error(
                        "Restart file element count mismatch."
                    );
                }
                if (var_check != 5) {
                    throw std::runtime_error(
                        "Restart file variable count mismatch."
                    );
                }

                input.numerical.maxiter += iter;
                sim.status.iteration = iter;
                sim.status.time = time;

                for (int i = 0; i < mesh.n_elements; ++i) {
                    for (int v = 0; v < 5; ++v) {
                        file >> fields.W(i, v);
                        if (!file) {
                            std::stringstream ss;
                            ss << "Error reading restart data at element "
                               << i << " variable " << v;
                            throw std::runtime_error(ss.str());
                        }
                    }
                }
                break;
            } else if (line.compare(0, 19, "# EULERCPP BIN File") == 0) {
                found_restart = true;
                Logger::debug() << "Detected binary restart file.";

                int iter;
                double time;
                int elm_check, var_check;

                file >> iter >> time >> elm_check >> var_check;
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (!file) {
                    throw std::runtime_error(
                        "Error reading binary restart file header values."
                    );
                }
                if (elm_check != mesh.n_elements) {
                    throw std::runtime_error(
                        "Restart file element count mismatch."
                    );
                }
                if (var_check != 5) {
                    throw std::runtime_error(
                        "Restart file variable count mismatch."
                    );
                }

                input.numerical.maxiter += iter;
                sim.status.iteration = iter;
                sim.status.time = time;

                file.read(reinterpret_cast<char*>(fields.Wdata()),
                        mesh.n_elements * 5 * sizeof(double));

                if (!file) {
                    throw std::runtime_error(
                        "Error reading binary restart file data."
                    );
                }
                break;
            }
        }

        if (!found_restart) {
            throw std::runtime_error("Restart file header not found.");
        }

        Logger::debug() << "Restart file loaded.";

    } else {
        Logger::debug() << "Loading initial conditions from input file...";

        #pragma omp parallel for
        for (int i = 0; i < mesh.n_elements; ++i) {
            for (int v = 0; v < 5; ++v) {
                fields.W(i, v) = input.init.W0[v];
            }
        }

        for (int b = 0; b < input.init.n_blocks; ++b) {
            Logger::debug() << "Loading initial conditions for box "
                            << b << ".";
            const auto& box = input.init.blocks[b];
            #pragma omp parallel for
            for (int i = 0; i < mesh.n_elements; ++i) {
                const auto& centroid = mesh.elements[i].centroid;
                if (centroid[0] >= box.xmin && centroid[0] <= box.xmax &&
                    centroid[1] >= box.ymin && centroid[1] <= box.ymax &&
                    centroid[2] >= box.zmin && centroid[2] <= box.zmax &&
                    math::distance(centroid, box.center) < box.radius) {
                    for (int v = 0; v < 5; ++v) {
                        fields.W(i, v) = box.W0[v];
                    }
                }
            }
        }
    }
}

/**
 * @brief Initialize simulation components and numerical schemes.
 *
 * This function performs the following:
 * - Initializes the numerical limiter
 * - Selects the reconstruction scheme
 * - Configures axisymmetric physics if needed
 * - Sets up output format and output folder for results
 *
 * @param sim Reference to the Simulation object to initialize.
 */
void initialize_simulation(Simulation& sim) {
    auto& input = sim.input;

    math::init_limiter(input.numerical.limiter);

    math::init_reconstruction(input.numerical.reconstruction);

    physics::init_riemann(input.numerical.riemann);

    if (input.physics.dimension == 2) {
        physics::init_axisymmetry(sim);
        Logger::info() << "Simulation set to axisymmetric mode.";
    }

    Writer::configure(
        input.output.output_format,
        input.output.restart_format,
        input.output.output_folder,
        input.output.output_name
    );

    if (input.output.n_probes > 0)
        Writer::init_probes(sim);

    if (input.output.n_reports > 0)
        Writer::init_reports(sim);
}

} // namespace eulercpp
