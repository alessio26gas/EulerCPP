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
 * @file solve.hpp
 * @brief Declaration of the main solver function for EulerCPP simulations.
 *
 * This file provides the interface for running the time-stepping solver,
 * updating fields, computing fluxes, applying boundary conditions and
 * corrections, and writing output periodically.
 *
 * The solver handles multi-stage time integration and supports graceful
 * termination and restart functionality.
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/simulation/solve.hpp>

namespace eulercpp {

/**
 * @brief Perform the main time-stepping solver loop.
 *
 * This function advances the simulation by iteratively updating the solution
 * fields. It performs the following steps in each iteration:
 * - Prepare solution for update
 * - Update timestep
 * - Update source terms
 * - Compute gradients and reconstructions
 * - Compute fluxes and apply boundary conditions
 * - Advance solution in time
 * - Apply physical corrections
 * - Print residuals and save output periodically
 *
 * The solver respects maximum iteration count, maximum simulation time,
 * and allows for early stopping via signal handling.
 *
 * @param sim Reference to the `Simulation` object to solve.
 */
void solve(Simulation& sim);

} //namespace eulercpp
