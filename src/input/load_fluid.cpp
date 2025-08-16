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
    @file load_fluid.cpp
    @brief Loads fluid properties from the simulation configuration file.

    This source file contains the function responsible for extracting
    the fluid-related parameters from a key-value configuration map.

    Expected keys:
      - "R"     : Gas constant for the fluid.
      - "gamma" : Ratio of specific heats.

    Missing keys leave the fluid properties at their default values.

    @see input.hpp, input_helpers.hpp, load_fluid.hpp
    @author Alessio Improta
 */

#include <map>
#include <string>

#include <eulercpp/input/input.hpp>
#include <eulercpp/input/input_helpers.hpp>
#include <eulercpp/input/load_fluid.hpp>

namespace eulercpp {

/**
 * @brief Populate fluid properties from a configuration map.
 *
 * Reads the gas constant (R) and specific heat ratio (gamma) if they exist.
 *
 * @param config Map of configuration key-value pairs.
 * @param input  Input structure to update with fluid properties.
 */
void load_fluid(
    const std::map<std::string, std::string>& config, Input& input
) {
    auto it = config.find("R");
    if (it != config.end()) input.fluid.R = std::stod(it->second);

    it = config.find("gamma");
    if (it != config.end()) input.fluid.gamma = std::stod(it->second);
}

} // namespace eulercpp
