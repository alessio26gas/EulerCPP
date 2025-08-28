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
 * @file probes.cpp
 * @brief Implements initialization and data collection for flow probes.
 *
 * Probes are located at user-specified locations in the domain.
 * Each probe is assigned to the nearest mesh element centroid. During the
 * simulation, primitive flow quantities are sampled and written to a CSV
 * file over time.
 *
 * @author Alessio Improta
 */

#include <fstream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <omp.h>

#include <eulercpp/math/vectors.hpp>
#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp::probes {

/**
 * @brief Initialize probes and assign each to its closest mesh element.
 *
 * For every probe defined in the input file, the nearest element centroid
 * is found (parallelized with OpenMP). A CSV file is opened and the header
 * line is written.
 *
 * @param sim Simulation object containing mesh and input data.
 * @param filepath Output path (without extension) for the probes CSV file.
 * @return An open output file stream ready for writing probe data.
 *
 * @throws std::runtime_error If the output file cannot be opened.
 */
std::ofstream init_probes(Simulation& sim, const std::string& filepath) {
    Logger::debug() << "Initializing probes...";

    const Mesh& mesh = sim.mesh;
    Input& input = sim.input;

    for (auto& probe : input.output.probes) {
        auto& loc = probe.location;
        double min_dist = std::numeric_limits<double>::max();

        #pragma omp parallel
        {
            double local_min_dist = std::numeric_limits<double>::max();
            int local_i = 0;

            #pragma omp for nowait
            for (int i = 0; i < mesh.n_elements; ++i) {
                auto& c = mesh.elements[i].centroid;
                double dist = math::distance(c, loc);
                if (dist < local_min_dist) {
                    local_min_dist = dist;
                    local_i = i;
                }
            }

            #pragma omp critical
            {
                if (local_min_dist < min_dist) {
                    probe.element = local_i;
                }
            }
        }
    }

    std::ofstream ofs(filepath + ".csv");
    if (!ofs) {
        Logger::warning() << "Failed to open file: " << filepath << ".csv";
        throw std::runtime_error("Failed to open probes file.");
    }

    ofs << "time,X,Y,Z,Density,VelocityX,VelocityY,VelocityZ,"
           "Pressure,Temperature,Mach\n";

    ofs << std::scientific << std::setprecision(7);

    return ofs;
}

/**
 * @brief Write flow field quantities at all probe locations.
 *
 * Data is written in scientific notation with 7-digit precision.
 *
 * @param sim Constant reference to the simulation object.
 * @param ofs Reference to the probes output file stream.
 */
void write_probes(const Simulation& sim, std::ofstream& ofs) {
    Logger::debug() << "Saving probes data...";

    const Mesh& mesh = sim.mesh;
    const Fields& fields = sim.fields;
    const Input& input = sim.input;

    const float R = input.fluid.R;
    const float gam = input.fluid.gamma;
    const float t = sim.status.time;

    for (auto& probe : input.output.probes) {
        const auto& c = mesh.elements[probe.element].centroid;
        const int i = probe.element;

        const float rho = fields.W(i, 0);
        const float u = fields.W(i, 1) / rho;
        const float v = fields.W(i, 2) / rho;
        const float w = fields.W(i, 3) / rho;
        const float V2 = u*u + v*v + w*w;
        const float p = (gam-1.0)*(fields.W(i, 4) - 0.5*rho*V2);
        const float T = p / rho / R;
        const float M = std::sqrt(V2 / (gam * R * T));

        ofs << t << "," << c[0] << "," << c[1] << "," << c[2] << ","
            << rho << "," << u << "," << v << "," << w << ","
            << p << "," << T << "," << M << "\n";
    }
}

} // namespace eulercpp::probes
