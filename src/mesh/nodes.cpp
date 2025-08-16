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
 * @file nodes.cpp
 * @brief Implements functions for reading mesh nodes.
 *
 * This file provides the implementation for reading the `$Nodes` section
 * of a mesh file. Each node is read into a Node structure and stored in
 * the Mesh object's node vector.
 * 
 * Error handling ensures proper file format and node data consistency.
 * 
 * @see mesh.hpp, nodes.hpp, logger.hpp
 * @author Alessio
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cstring>

#include <eulercpp/mesh/mesh.hpp>
#include <eulercpp/mesh/nodes.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp {

/**
 * @brief Reads the node data from a mesh file into the mesh structure.
 *
 * This function searches for the `$Nodes` section in the mesh file and
 * parses node information (ID and XYZ coordinates) into the mesh's 
 * `nodes` vector.
 *
 * @param file Reference to an input file stream positioned at the beginning 
 * of the mesh file.
 * @param mesh Reference to the Mesh structure where node data will be stored.
 *
 * @throws std::runtime_error If reading the number of nodes or individual 
 * node data fails.
 *
 * @note The mesh file must follow a standard format where node data follows
 *       the `$Nodes` tag and includes one node per line in the format:
 *       `<id> <x> <y> <z>`.
 */
void read_nodes(std::ifstream& file, Mesh& mesh) {
    Logger::debug() << "Reading nodes...";

    std::string line;
    char buffer[128];

    while (file.getline(buffer, sizeof(buffer))) {
        if (std::strncmp(buffer, "$Nodes", 6) == 0) {
            if (!file.getline(buffer, sizeof(buffer))) {
                throw std::runtime_error("Could not read number of nodes.");
            }

            mesh.n_nodes = std::strtol(buffer, nullptr, 10);
            if (mesh.n_nodes <= 0) {
                throw std::runtime_error("No nodes found.");
            }
            mesh.nodes.resize(mesh.n_nodes);

            for (int i = 0; i < mesh.n_nodes; ++i) {
                if (!file.getline(buffer, sizeof(buffer))) {
                    std::ostringstream oss;
                    oss << "Unexpected end of file while reading node data at node index " << i;
                    throw std::runtime_error(oss.str());
                }

                char* ptr = buffer;
                mesh.nodes[i].id = std::strtol(ptr, &ptr, 10);
                for (int d = 0; d < 3; ++d) {
                    mesh.nodes[i].position[d] = std::strtod(ptr, &ptr);
                }
            }

            Logger::info() << "Read " << mesh.n_nodes << " nodes.";
            return;
        }
    }

    throw std::runtime_error("No $Nodes section found in mesh file.");
}

} // namespace eulercpp
