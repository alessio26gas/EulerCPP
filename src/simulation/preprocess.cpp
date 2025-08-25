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
 * @file preprocess.cpp
 * @brief Implements preprocessing for EulerCPP simulations.
 *
 * This file contains the implementation of the `preprocess` function, which
 * initializes the simulation before the main solver loop. It handles:
 * - Field allocation and initialization
 * - Initial condition setup
 * - Boundary condition setup
 * - Signal handler setup for safe termination
 * - Writing initial state to output
 *
 * @author Alessio Improta
 */

#include <ctime>
#include <iostream>

#include <eulercpp/input/input.hpp>
#include <eulercpp/mesh/mesh.hpp>
#include <eulercpp/simulation/initialization.hpp>
#include <eulercpp/simulation/preprocess.hpp>
#include <eulercpp/simulation/signal_handler.hpp>
#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/physics/boundaries.hpp>
#include <eulercpp/output/logger.hpp>
#include <eulercpp/output/writer.hpp>
#include <eulercpp/math/time_utils.hpp>

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
void preprocess(Simulation& sim) {
    clock_t start = clock();

    Input& input = sim.input;
    Mesh& mesh = sim.mesh;
    Fields& fields = sim.fields;
    
    Logger::debug() << "Setting up signal handling...";
    setup_signal_handler(sim);
    Logger::debug() << "Signal handling set up.";

    Logger::debug() << "Initializing fields...";
    fields.init(mesh, input);
    Logger::info() << "Fields initialized.";

    Logger::debug() << "Initializing simulation...";
    initialize_simulation(sim);
    Logger::info() << "Simulation initialized.";

    Logger::debug() << "Setting initial conditions...";
    set_initial_conditions(sim);
    Logger::info() << "Initial conditions set.";

    Logger::debug() << "Initializing boundary conditions...";
    physics::init_boundaries(sim);
    Logger::info() << "Boundary conditions set.";

    Logger::debug() << "Writing initial conditions...";
    Writer::save_solution(sim);

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    Logger::success() << "Preprocessing complete. ("
                      << math::format_duration(elapsed) << ")";
}

} // namespace eulercpp
