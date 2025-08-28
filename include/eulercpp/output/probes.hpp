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
 * @file probes.hpp
 * @brief Header file for flow probes.
 *
 * Probes are located at user-specified locations in the domain.
 * Each probe is assigned to the nearest mesh element centroid. During the
 * simulation, primitive flow quantities are sampled and written to a CSV
 * file over time.
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/simulation/simulation.hpp>

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
std::ofstream init_probes(Simulation& sim, const std::string& filepath);

/**
 * @brief Write flow field quantities at all probe locations.
 *
 * Data is written in scientific notation with 7-digit precision.
 *
 * @param sim Constant reference to the simulation object.
 * @param ofs Reference to the probes output file stream.
 */
void write_probes(const Simulation& sim, std::ofstream& ofs);

} // namespace eulercpp::probes
