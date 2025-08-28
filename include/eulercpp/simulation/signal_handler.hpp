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
 * @file signal_handler.hpp
 * @brief Provides signal handling for graceful simulation termination.
 *
 * This header defines functions to handle operating system signals
 * to stop the simulation cleanly. It allows the current iteration
 * to finish before exiting, ensuring proper cleanup and logging.
 *
 * @author Alessio Improta
 */

#pragma once

#include <csignal>
#include <atomic>

#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp {

/**
 * @brief Global pointer to the simulation object for signal handling.
 */
static Simulation* g_sim_ptr = nullptr;

/**
 * @brief Signal handler to gracefully stop the simulation.
 *
 * This function sets the stopped flag in the simulation status to true.
 * It allows the simulation to finish the current iteration and exit cleanly.
 *
 * @param signum Signal number.
 */
void signal_handler(int signum) {
    Logger::warning() << "Stop signal received.";
    if (g_sim_ptr) g_sim_ptr->status.stopped = true;
}

/**
 * @brief Setup signal handling for the simulation.
 *
 * @param sim Reference to the Simulation object.
 */
void setup_signal_handler(Simulation& sim) {
    g_sim_ptr = &sim;
    std::signal(SIGINT, signal_handler);
}

} // namespace eulercpp
