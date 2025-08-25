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
 * @file reports.cpp
 * @brief Implements output of global reports.
 *
 * Reports are global integral quantities evaluated on user-defined boundary 
 * patches. For each report, the code integrates fluxes across all faces with 
 * the specified boundary tag, then outputs total mass flow rate, forces, and
 * moments at the chosen reference center of gravity (cg).
 *
 * @author Alessio Improta
 */

#include <fstream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <omp.h>

#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp::reports {

/**
 * @brief Initialize reports and open CSV output file.
 *
 * This function creates a CSV file for storing global integral quantities 
 * (mass flow, forces, and moments). The file header is written, and the 
 * stream is returned to the caller for use in subsequent timesteps.
 *
 * @param sim Constant reference to the simulation object.
 * @param filepath Output path (without extension) for the reports CSV file.
 * @return std::ofstream An open file stream ready for writing reports data.
 *
 * @throws std::runtime_error If the file cannot be opened.
 */
std::ofstream init_reports(const Simulation& sim, const std::string& filepath) {
    Logger::debug() << "Initializing reports...";

    const Mesh& mesh = sim.mesh;
    const Input& input = sim.input;

    std::ofstream ofs(filepath + ".csv");
    if (!ofs) {
        Logger::warning() << "Failed to open file: " << filepath << ".csv";
        throw std::runtime_error("Failed to open reports file.");
    }

    ofs << "time,boundary,mdot,Fx,Fy,Fz,Mx,My,Mz\n";

    ofs << std::scientific << std::setprecision(7);

    return ofs;
}

/**
 * @brief Write global reports data to the CSV file at the current timestep.
 *
 * Results are written to the CSV file in scientific notation with 7-digit
 * precision.
 *
 * @param sim Constant reference to the simulation object.
 * @param ofs Reference to the reports output file stream.
 */
void write_reports(const Simulation& sim, std::ofstream& ofs) {
    Logger::debug() << "Saving reports...";

    const Mesh& mesh = sim.mesh;
    const Fields& fields = sim.fields;
    const Input& input = sim.input;

    const float time = sim.status.time;

    for (auto& report : input.output.reports) {
        const auto& cg = report.cg;
        const int b = report.boundary;

        float mdot = 0.0;
        std::array<float, 3> F = {0.0};
        std::array<float, 3> M = {0.0};

        for (int f = 0; f < mesh.n_faces; ++f) {
            const auto& face = mesh.faces[f];
            if (face.flag == b) {
                const auto& c = face.centroid;
                mdot += fields.F(f, 0);
                for (int dim = 0; dim < 3; ++dim) {
                    F[dim] += fields.F(f, dim+1);
                    M[dim] += F[dim] * (c[dim]-cg[dim]);
                }
            }
        }

        ofs << time << "," << b+1 << "," << mdot << ","
            << F[0] << "," << F[1] << "," << F[2] << ","
            << M[0] << "," << M[1] << "," << M[2] << "\n";
    }
}

} // namespace eulercpp::reports
