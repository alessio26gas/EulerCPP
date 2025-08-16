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
 * @file sources.hpp
 * @brief Declaration of source term update routines.
 *
 * This file declares functions that compute source terms for the Euler
 * equations, including handling of axisymmetric contributions.
 * Source terms are applied per element and scaled by element volume.
 * 
 * Parallel computation using OpenMP is supported.
 * 
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/simulation/simulation.hpp>

namespace eulercpp::physics {

/**
 * @brief Compute and update source terms for the simulation.
 *
 * This function resets all source terms to zero, applies any physics-specific
 * source contributions and scales the sources by the volume of each element.
 *
 * @param sim Reference to the Simulation object.
 */
void update_sources(Simulation& sim);

} // namespace eulercpp::physics
