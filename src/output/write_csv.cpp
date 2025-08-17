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
 * @file write_csv.cpp
 * @brief Implementation of functions to save simulation data in CSV format.
 *
 * @author Alessio Improta
 */

#include <fstream>
#include <cmath>
#include <iomanip>

#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp {

/**
 * @brief Write simulation data to a CSV file.
 *
 * @param sim Simulation object containing mesh and field data
 * @param filepath Path to the output CSV file
 */
void write_csv(const Simulation& sim, const std::string& filepath) {
    Logger::info() << "Saving solution as CSV...";

    const Mesh& mesh = sim.mesh;
    const Fields& fields = sim.fields;
    const Input& input = sim.input;

    std::ofstream ofs(filepath + ".csv");
    if (!ofs) {
        Logger::warning() << "Failed to open file: " << filepath << ".csv";
        return;
    }

    ofs << std::scientific << std::setprecision(7);

    ofs << "X,Y,Z,Density,VelocityX,VelocityY,VelocityZ,"
           "Pressure,Temperature,Mach\n";

    const float R = input.fluid.R;
    const float gam = input.fluid.gamma;

    for (int i = 0; i < mesh.n_elements; ++i) {
        const auto& c = mesh.elements[i].centroid;
        const float rho = fields.W(i, 0);
        const float u = fields.W(i, 1) / rho;
        const float v = fields.W(i, 2) / rho;
        const float w = fields.W(i, 3) / rho;
        const float V2 = u*u + v*v + w*w;
        const float p = (gam-1.0)*(fields.W(i, 4) - 0.5*rho*V2);
        const float T = p / rho / R;
        const float M = std::sqrt(V2 / (gam * R * T));

        ofs << c[0] << "," << c[1] << "," << c[2] << ","
            << rho << "," << u << "," << v << "," << w << ","
            << p << "," << T << "," << M << "\n";
    }

    ofs.close();
}

} // namespace eulercpp
