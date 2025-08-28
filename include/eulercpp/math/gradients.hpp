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
 * @file gradients.hpp
 * @brief Functions to compute gradients of field variables in the mesh.
 *
 * Provides routines to compute spatial gradients variables on a finite-volume
 * mesh. The main function `compute_gradients()` calculates gradients for all
 * elements using neighboring elements and face weights.
 *
 * The computation uses OpenMP for parallelization over mesh elements.
 *
 * @see gradients.cpp for implementation details.
 */

#pragma once

namespace eulercpp {
    struct Simulation;
}

namespace eulercpp::math {

/**
 * @brief Compute gradients of conserved variables in a simulation.
 *
 * Computes the gradients of all variables stored in `sim.fields.W`
 * for every element of the mesh. Gradients are calculated using
 * neighboring elements and face-weight vectors and stored in
 * `sim.fields.gradW`.
 *
 * @param sim Reference to the Simulation containing mesh and fields.
 *
 * @note The computation is parallelized over mesh elements using OpenMP.
 */
void compute_gradients(eulercpp::Simulation& sim);

} // namespace eulercpp::math
