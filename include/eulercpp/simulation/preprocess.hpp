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
 * @file preprocess.hpp
 * @brief Declaration of preprocessing functions for EulerCPP simulations.
 *
 * This file provides the interface for initializing the simulation fields,
 * applying initial conditions, setting up boundaries, and enabling signal
 * handling.
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/simulation/simulation.hpp>

namespace eulercpp {

/**
 * @brief Preprocess the simulation by setting up fields, boundaries, and
 * initial conditions.
 *
 * The `preprocess` function performs all necessary steps to prepare the
 * simulation before the main time integration. This includes:
 * 1. Setting up signal handling for graceful exit
 * 2. Allocating and initializing all fields
 * 3. Initializing the simulation status and variables
 * 4. Applying initial conditions
 * 5. Initializing boundary conditions
 * 6. Writing the initial simulation state to output files
 *
 * The function also measures the preprocessing time and logs messages at
 * each step to provide detailed feedback.
 *
 * @param sim Reference to the `Simulation` object to preprocess.
 */
void preprocess(Simulation& sim);

} // namespace eulercpp
