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
    @file load_logger.cpp
    @brief Loads Logger-related parameters from the configuration map.

    This function reads verbosity level and log file paths from a key-value
    map and configures the Logger accordingly.

    Expected keys:
      - "verbosity" : integer code corresponding to Logger::Level.
         - 0 : SILENT
         - 1 : ERROR
         - 2 : WARNING
         - 3 : SUCCESS
         - 4 : INFO
         - 5 : DEBUG
      - "log_file"  : path to the main log file.
      - "rhs_file"  : path to the RHS log file.

    Missing keys leave the logger at default settings.

    @author Alessio Improta
 */

#include <map>
#include <string>

#include <eulercpp/input/input.hpp>
#include <eulercpp/input/load_logger.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp {

/**
 * @brief Configure the Logger from a key-value configuration map.
 *
 * Reads verbosity level and optional log file paths.
 *
 * @param config Map of string key-value configuration parameters.
 * @param input  Input structure (included for interface consistency).
 */
void load_logger(
    const std::map<std::string, std::string>& config, Input& input
) {
    auto it = config.find("verbosity");
    if (it != config.end())
        Logger::set_verbosity(
            static_cast<Logger::Level>(std::stoi(it->second))
        );
    
    it = config.find("log_file");
    if (it != config.end()) {
        Logger::set_log_file(it->second);
    }

    it = config.find("rhs_file");
    if (it != config.end()) {
        Logger::set_rhs_file(it->second);
    }
}

} // namespace eulercpp
