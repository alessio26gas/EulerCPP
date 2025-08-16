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
 * @file restart.hpp
 * @brief Functionality for writing simulation restart files in eulercpp.
 *
 * This header defines utilities to save the current simulation state
 * (mesh, fields, iteration, and time) to a restart file.
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/simulation/simulation.hpp>

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
void write_restart(const Simulation& sim, const std::string& filepath);

} // namespace eulercpp
