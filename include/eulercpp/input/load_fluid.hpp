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
 * @file load_fluid.hpp
 * @brief Fluid properties loading interface.
 *
 * This header defines the Fluid struct that contains all the physical 
 * properties of the working fluid for the simulation. It also declares 
 * the function that populates these properties from a configuration map.
 *
 * @author Alessio Improta
 */

#pragma once

#include <array>
#include <map>
#include <string>
#include <vector>

namespace eulercpp {

struct Input;

/**
 * @struct Fluid
 * @brief Contains fluid physical properties.
 *
 * Fields:
 * - `R` : Gas constant [J/(kg·K)]
 * - `gamma` : Specific heat ratio
 */
struct Fluid {
    double R = 287.0;      /**< Gas constant [J/(kg·K)] */
    double gamma = 1.4;    /**< Specific heat ratio */
};

/**
 * @brief Populate fluid properties from a configuration map.
 *
 * Reads the gas constant (R) and specific heat ratio (gamma) if they exist.
 *
 * @param config Map of configuration key-value pairs.
 * @param input  Input structure to update with fluid properties.
 */
void load_fluid(const std::map<std::string, std::string>& config, Input& input);

} // namespace eulercpp
