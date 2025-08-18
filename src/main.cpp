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
    @file main.cpp
    @brief Main program entry point.

    This file defines the 'main()' function, which serves as the control hub 
    for the EulerCPP solver. It is responsible for:
      - Displaying the application banner and version details.
      - Parsing input files and command-line arguments.
      - Loading and preparing the computational mesh.
      - Initializing numerical data structures.
      - Executing the solver loop until convergence or termination.
      - Handling all top-level exceptions and logging errors.

    This is the first code executed when the program starts, coordinating
    the entire simulation workflow from initialization to final output.

    @author Alessio Improta
 */

#include <cstdlib>
#include <iostream>

#include <eulercpp/input/input.hpp>
#include <eulercpp/mesh/mesh.hpp>
#include <eulercpp/simulation/preprocess.hpp>
#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/simulation/solve.hpp>
#include <eulercpp/output/logger.hpp>

using namespace eulercpp;

/**
 * @brief Main program entry point.
 *
 * Runs the full CFD workflow: loads input, prepares the mesh,
 * initializes the solver, executes iterations, and handles errors.
 *
 * @param argc Number of CLI arguments.
 * @param argv CLI argument values.
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
int main(int argc, char* argv[]) {

    /// @brief Print the EulerCPP ASCII art banner.
    Logger::info() << "";
    Logger::info() << " ___ _   _ _    ___ ___  \e[1;96m___ ___ ___ ";
    Logger::info() << "| __| | | | |  | __| _ \\\e[1;96m/ __| _ \\ _ \\";
    Logger::info() << "| _|| |_| | |__| _||   / \e[1;96m(__|  _/  _/";
    Logger::info() << "|___|\\___/|____|___|_|_\\\e[1;96m\\___|_| |_|  ";
    Logger::info() << "";
    Logger::info() << "EulerCPP - A C++ 3D Euler CFD solver";
    Logger::info() << "Version 0.3.0 (c) 2025 Alessio Improta";
    Logger::info() << "";

    /// @brief Main simulation object containing the entire simulation state.
    Simulation sim;

    try {
        /// @brief Read input parameters from input file.
        load_input(sim.input, argc, argv);

        /// @brief Load the mesh and compute geometric properties.
        read_mesh(sim);

        /// @brief Allocate memory and initialize the simulation state.
        preprocess(sim);

        /// @brief Execute the solver loop until convergence or termination.
        solve(sim);

    } catch (const std::exception& e) {
        /// @brief Log any exceptions encountered during simulation.
        Logger::error() << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
