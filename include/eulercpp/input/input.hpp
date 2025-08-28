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
 * @file input.hpp
 * @brief Defines the structure and functions for handling simulation input.
 *
 * This file declares the main `Input` structure, which aggregates all
 * input parameters required for a simulation run. It includes physics,
 * mesh, fluid properties, numerical settings, initial conditions,
 * boundary conditions, and output configuration.
 *
 * The `load_input` function provides a convenient entry point to
 * populate the `Input` structure from configuration files or command-
 * line arguments.
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/input/load_input.hpp>

namespace eulercpp {

/**
 * @struct Input
 * @brief Holds all input parameters for the simulation.
 *
 * This structure aggregates all the configuration settings needed
 * to initialize and run a simulation, including:
 * - Physics parameters
 * - Mesh settings
 * - Fluid properties
 * - Numerical solver parameters
 * - Initial conditions
 * - Boundary conditions
 * - Output settings
 */
struct Input {
    Physics physics;        /**< Physics input settings. */
    MeshSettings mesh;      /**< Mesh input settings. */
    Fluid fluid;            /**< Fluid properties input settings. */
    Numerical numerical;    /**< Numerical settings. */
    InitialConditions init; /**< Initial conditions. */
    BoundaryConditions bc;  /**< Boundary conditions. */
    OutputSettings output;  /**< Output settings. */
};

/**
 * @brief Validates CLI arguments and loads simulation input.
 *
 * This function ensures that a valid configuration file is provided
 * via the command line, verifies its accessibility, measures the time
 * taken to process it, and logs relevant diagnostic messages.
 *
 * @param input Reference to the Input structure to populate.
 * @param argc  Number of command-line arguments.
 * @param argv  Array of command-line argument strings.
 * @throws std::invalid_argument if arguments are missing or file is invalid.
 */
void load_input(Input& input, int argc, char* argv[]);

} // namespace eulercpp
