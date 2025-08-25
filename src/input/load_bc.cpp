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
 * @file load_bc.cpp
 * @brief Loads boundary condition definitions from configuration.
 *
 * This source file reads boundary-condition settings from a key–value map
 * and initializes the corresponding fields.
 *
 * Expected keys:
 *  - "n_boundaries" : number of boundary regions.
 *  - For each boundary b (1-based):
 *      - "bc_b"      : boundary type (physics::BCType code).
 *      - "bc_b_id"   : boundary identifier.
 *      - "bc_b_xmin", "bc_b_ymin", "bc_b_zmin", 
 *        "bc_b_xmax", "bc_b_ymax", "bc_b_zmax",
 *        "bc_b_radius", "bc_b_center" : spatial extents.
 *      - "bc_b_var_j": value for variable j, 1 ≤ j ≤ 5.
 *
 * @see load_bc.hpp, input.hpp
 * @author Alessio Improta
 */

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <eulercpp/input/input.hpp>
#include <eulercpp/input/load_bc.hpp>
#include <eulercpp/input/input_helpers.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp {

/**
 * @brief Populate boundary conditions from a configuration map.
 *
 * This function reads boundary configuration parameters from
 * a map of string key–value pairs and updates the `Input` structure.
 *
 * @param config Map of configuration key–value pairs.
 * @param input  Input structure to update.
 */
void load_bc(const std::map<std::string, std::string>& config, Input& input) {
    /// Parse number of boundaries
    auto it = config.find("n_boundaries");
    if (it != config.end())
        input.bc.n_boundaries = std::stoi(it->second);

    input.bc.boundaries.resize(input.bc.n_boundaries);
    Logger::debug() << "Number of boundaries: " << input.bc.n_boundaries;

    /// Loop over each boundary definition
    for (int b = 0; b < input.bc.n_boundaries; ++b) {
        auto& boundary = input.bc.boundaries[b];

        /// Helper to build key names
        auto key_for = [&](const std::string& prefix) {
            return prefix + "_" + std::to_string(b + 1);
        };

        /// Boundary type (enum code)
        if (auto it = config.find(key_for("bc")); it != config.end()) {
            boundary.type = static_cast<physics::BCType>(std::stoi(it->second));
        }

        /// Boundary ID
        if (auto it = config.find(key_for("bc") + "_id"); it != config.end()) {
            boundary.id = std::stoi(it->second);
        }

        /// Spatial extents
        if (auto it = config.find(key_for("bc") + "_xmin"); it != config.end())
            boundary.xmin = std::stod(it->second);
        if (auto it = config.find(key_for("bc") + "_xmax"); it != config.end())
            boundary.xmax = std::stod(it->second);
        if (auto it = config.find(key_for("bc") + "_ymin"); it != config.end())
            boundary.ymin = std::stod(it->second);
        if (auto it = config.find(key_for("bc") + "_ymax"); it != config.end())
            boundary.ymax = std::stod(it->second);
        if (auto it = config.find(key_for("bc") + "_zmin"); it != config.end())
            boundary.zmin = std::stod(it->second);
        if (auto it = config.find(key_for("bc") + "_zmax"); it != config.end())
            boundary.zmax = std::stod(it->second);
        if (auto it = config.find(key_for("bc") + "_radius"); it != config.end())
            boundary.radius = std::stod(it->second);
        if (auto it = config.find(key_for("bc") + "_center"); it != config.end()) {
            auto center = parse_vector(it->second);
            if (center.size() > 3) {
                throw std::invalid_argument("Invalid boundary center coordinates.");
            }
            for (int dim = 0; dim < center.size(); ++dim) {
                boundary.center[dim] = center[dim];
            }
        }

        /// Parse variable values for each variable
        for (int j = 0; j < 5; ++j) {
            std::string var_key = key_for("bc") + "_var_" + std::to_string(j+1);
            if (auto it = config.find(var_key); it != config.end()) {
                boundary.value[j] = std::stod(it->second);
            }
        }
    }
}

} // namespace eulercpp
