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
 * @file load_physics.hpp
 * @brief Declares structures and functions for physics input settings.
 *
 * Defines the Physics struct, which holds all physics parameters for
 * the simulation. Declares load_physics() to populate these parameters 
 * from a configuration map into the global Input structure.
 *
 * @author Alessio Improta
 */

#pragma once

#include <map>
#include <string>

namespace eulercpp {

struct Input;

/**
 * @struct Physics
 * @brief Holds all physics-related input settings.
 */
struct Physics {
    int dimension = 0; /**< Spatial dimension. */
};

/**
 * @brief Populate physics parameters from a configuration map.
 *
 * Expected keys:
 *   - "dimension" : integer (2 or 3).
 *         Accepts numeric codes or textual equivalents:
 *             0 → 1D
 *             1 → 2D
 *             2 → Axisymmetric
 *             3 → 3D
 *
 * If a key is absent, the corresponding field is left unchanged (keeps
 * its default value). Invalid numeric strings may throw from `std::stoi`.
 *
 * @param config Map of configuration key–value pairs (strings).
 * @param input  Input structure to be updated in-place.
 */
void load_physics(
    const std::map<std::string, std::string>& config, Input& input
);

} // namespace eulercpp
