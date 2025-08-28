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
 * @file reports.hpp
 * @brief Header file for global reports.
 *
 * Reports are global integral quantities evaluated on user-defined boundary
 * patches. For each report, the code integrates fluxes across all faces with
 * the specified boundary tag, then outputs total mass flow rate, forces, and
 * moments at the chosen reference center of gravity (cg).
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/simulation/simulation.hpp>

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
std::ofstream init_reports(const Simulation& sim, const std::string& filepath);

/**
 * @brief Write global reports data to the CSV file at the current timestep.
 *
 * Results are written to the CSV file in scientific notation with 7-digit
 * precision.
 *
 * @param sim Constant reference to the simulation object.
 * @param ofs Reference to the reports output file stream.
 */
void write_reports(const Simulation& sim, std::ofstream& ofs);

} // namespace eulercpp::reports
