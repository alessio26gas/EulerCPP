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
 * @file fluxes.hpp
 * @brief Declarations for flux computation.
 *
 * Provides functions to compute convective fluxes across mesh faces using
 * Riemann solvers and mapping fluxes back to the element coordinate system.
 *
 * Supports parallel execution using OpenMP.
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/simulation/simulation.hpp>

namespace eulercpp::physics {

/**
 * @brief Compute convective fluxes across all mesh faces.
 *
 * For each face, the function:
 * 1. Extracts the left and right states in the face-normal coordinate system.
 * 2. Calls the Riemann solver to compute fluxes across the face.
 * 3. Maps the fluxes back to the global coordinate system.
 * 4. Scales the fluxes by the face area.
 *
 * This operation is parallelized using OpenMP.
 *
 * @param sim Reference to the Simulation object.
 */
void compute_fluxes(Simulation& sim);

} // namespace eulercpp::physics
