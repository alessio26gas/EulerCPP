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
 * @file load_logger.hpp
 * @brief Configuration of the simulation logger.
 *
 * This file declares the function to load logger settings from a 
 * configuration map, including verbosity levels and optional 
 * log file paths. The logger configuration is applied to the 
 * Input structure.
 *
 * @author Alessio Improta
 */

#pragma once

#include <map>
#include <string>

namespace eulercpp {

struct Input;

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
);

} // namespace eulercpp
