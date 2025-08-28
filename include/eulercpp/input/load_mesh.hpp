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
 * @file load_mesh.hpp
 * @brief Input handling for mesh settings.
 *
 * Declares the structure and function to load mesh parameters
 * from a key-value configuration map. Includes mesh file path
 * and minimum allowed cell volume.
 *
 * @author Alessio Improta
 */

#pragma once

#include <map>
#include <string>

namespace eulercpp {

struct Input;

/**
 * @struct MeshSettings
 * @brief Holds all input mesh settings.
 */
struct MeshSettings {
    std::string mesh_file;      /**< Mesh file path. */
    double min_volume = 0.0;    /**< Minimum allowed cell volume. */
};

/**
 * @brief Loads mesh-related settings from the configuration map.
 *
 * This function searches the provided key-value map for mesh-specific keys
 * and updates the global input parameters accordingly.
 *
 * Specifically, it looks for:
 * - "mesh_file" : Path or name of the mesh input file.
 * - "min_volume": Minimum allowed volume in the mesh.
 *
 * @param config A map containing all configuration keys and their string values.
 * @param input  Input structure to update with mesh parameters.
 */
void load_mesh(const std::map<std::string, std::string>& config, Input& input);

} // namespace eulercpp
