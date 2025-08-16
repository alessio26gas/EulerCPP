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
    @file input.cpp
    @brief Loads and parses simulation input parameters.

    This source file contains the functions responsible for reading,
    validating, and processing the simulation configuration file. It
    coordinates the extraction of settings for:
      - Logger configuration.
      - Physics model selection.
      - Mesh definition.
      - Fluid properties.
      - Numerical parameters.
      - Output settings.
      - Initial and boundary conditions.

    It provides two main functions:
      - read_input_file(): Reads and processes the configuration file.
      - load_input(): Orchestrates input file validation, timing, and
        execution of read_input_file().

    All diagnostic messages and errors are handled through the Logger
    system.

    @see input.hpp, input_helpers.hpp, load_input.hpp
    @see Logger, math::format_duration
    @author Alessio Improta
 */

#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include <eulercpp/input/input.hpp>
#include <eulercpp/input/input_helpers.hpp>
#include <eulercpp/input/load_input.hpp>
#include <eulercpp/output/logger.hpp>
#include <eulercpp/math/time_utils.hpp>

namespace eulercpp {

/**
 * @brief Reads and processes all simulation settings from a configuration file.
 *
 * This function parses the specified input file, extracting and applying
 * all solver parameters. The settings are loaded in the following order:
 *   1. Logger configuration.
 *   2. Physics parameters.
 *   3. Mesh information.
 *   4. Fluid properties.
 *   5. Numerical solver parameters.
 *   6. Output configuration.
 *   7. Initial conditions.
 *   8. Boundary conditions.
 *
 * @param filename Path to the configuration file.
 * @param input    Reference to the Input structure to populate.
 * @throws std::runtime_error if parsing fails or required keys are missing.
 */
void read_input_file(const std::string& filename, Input& input) {
    const auto config = parse_config_file(filename);

    /// Load logger settings.
    load_logger(config, input);
    Logger::debug() << "Loaded logger settings.";
    Logger::info() << "Verbosity level: " << Logger::get_verbosity() << ".";

    /// Load physics settings.
    Logger::debug() << "Loading physics settings...";
    load_physics(config, input);

    /// Load mesh settings.
    Logger::debug() << "Loading mesh settings...";
    load_mesh(config, input);

    /// Load fluid settings.
    Logger::debug() << "Loading fluid settings...";
    load_fluid(config, input);

    /// Load numerical settings.
    Logger::debug() << "Loading numerical settings...";
    load_numerical(config, input);

    /// Load output settings.
    Logger::debug() << "Loading output settings...";
    load_output(config, input);

    /// Load initial conditions.
    Logger::debug() << "Loading initial conditions...";
    load_init(config, input);

    /// Load boundary conditions.
    Logger::debug() << "Loading boundary conditions...";
    load_bc(config, input);
}

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
void load_input(Input& input, int argc, char* argv[]) {
    clock_t start = clock();

    /// Check for correct usage
    if (argc != 2) {
        throw std::invalid_argument(
            std::string("Usage: ") + argv[0] + " <input_file>"
        );
    }

    /// Check if the input file exists
    const std::string filename = argv[1];
    std::ifstream infile(filename);
    if (!infile) {
        throw std::invalid_argument("Cannot open input file " + filename);
    }
    infile.close();

    /// Read input file
    Logger::info() << "Loading input settings from " << filename;
    read_input_file(filename, input);

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    Logger::success() << "Input loaded. ("
                      << math::format_duration(elapsed) << ")";
}

} // namespace eulercpp
