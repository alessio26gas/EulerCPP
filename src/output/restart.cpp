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
 * @file restart.cpp
 * @brief Implementation of restart file writing functionality.
 *
 * This file implements functions for saving the current simulation state
 * to a restart file, allowing simulations to be resumed later.
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
 * @brief Writes a restart file for the given simulation.
 *
 * The restart file contains:
 * - Current iteration number.
 * - Simulation time.
 * - Number of elements in the mesh.
 * - Number of conserved variables.
 * - Field values for each element.
 *
 * @param sim The simulation object containing the mesh, fields, and status.
 * @param filepath Path to the restart file to write.
 *
 * @note Overwrites existing files with the same name.
 */
void write_restart(const Simulation& sim, const std::string& filepath) {
    Logger::info() << "Saving restart file...";

    const Mesh& mesh = sim.mesh;
    const Fields& fields = sim.fields;
    const Input& input = sim.input;

    std::ofstream ofs(filepath);
    if (!ofs) {
        Logger::warning() << "Failed to open file: " << filepath;
        return;
    }

    ofs << std::scientific << std::setprecision(7);

    ofs << "# EULERCPP Restart File\n";

    ofs << sim.status.iteration << "\n"
        << sim.status.time << "\n"
        << sim.mesh.n_elements << "\n"
        << 5 << "\n";

    for (int i = 0; i < sim.mesh.n_elements; i++) {
        for (int v = 0; v < 5; v++) {
            ofs << sim.fields.W(i, v) << " ";
        }
        ofs << "\n";
    }

    ofs.close();
}

} // namespace eulercpp
