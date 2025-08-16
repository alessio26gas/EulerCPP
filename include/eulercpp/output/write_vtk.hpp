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
 * @file write_vtk.hpp
 * @brief Functions to write simulation data in VTK format.
 *
 * This header provides functions for saving EulerCPP simulation results
 * to VTK files for visualization. Supports both ASCII and binary formats.
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/simulation/simulation.hpp>

namespace eulercpp {

/**
 * @brief Write simulation data to a VTK file in ASCII format.
 *
 * @param sim Simulation object containing mesh and field data
 * @param filepath Path to the output VTK file
 */
void write_vtk_ascii(const Simulation& sim, const std::string& filepath);

/**
 * @brief Write simulation data to a VTK file in binary format.
 *
 * @param sim Simulation object containing mesh and field data
 * @param filepath Path to the output VTK file
 */
void write_vtk_bin(const Simulation& sim, const std::string& filepath);

} // namespace eulercpp
