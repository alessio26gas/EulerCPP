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
 * @file nodes.hpp
 * @brief Defines the Node structure and functions to read mesh nodes.
 *
 * This header provides the Node structure used to represent a mesh node
 * in 3D space, including a unique identifier and Cartesian coordinates.
 * It also declares the function to read nodes from a mesh file into a Mesh.
 * 
 * @author Alessio Improta
 */

#pragma once

#include <array>
#include <fstream>
#include <string>
#include <vector>

namespace eulercpp {

struct Mesh;

/**
 * @struct Node
 * @brief Represents a node in the computational mesh.
 *
 * Each node has a unique identifier and a position in 3D space.
 */
struct Node {
    int id;                         /**< Unique identifier of the node. */
    std::array<double, 3> position; /**< Cartesian coordinates (x, y, z). */
};

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
void read_nodes(std::ifstream& file, Mesh& mesh);

} // namespace eulercpp
