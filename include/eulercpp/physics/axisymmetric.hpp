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
 * @file axisymmetric.hpp
 * @brief Declarations for axisymmetric mode handling.
 *
 * Provides functions to handle axisymmetric mode.
 * Parallel computation using OpenMP is supported.
 *
 * @author Alessio Improta
 */

#pragma once

namespace eulercpp {
    struct Simulation;
}

namespace eulercpp::physics {

/**
 * @brief Apply axisymmetric source terms to the simulation.
 *
 * Computes additional source contributions arising from axisymmetric mode.
 *
 * @param sim Reference to the Simulation object to be updated.
 */
void axisymmetry_sources(Simulation& sim);

/**
 * @brief Initialize the simulation mesh for axisymmetric calculations.
 *
 * Modifies the volume of each element and the area of each face by
 * multiplying with the radial coordinate to account for axisymmetry.
 *
 * @param sim Reference to the Simulation object to be initialized.
 */
void init_axisymmetry(Simulation& sim);

} // namespace eulercpp::physics
