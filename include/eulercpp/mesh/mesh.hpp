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
 * @file mesh.hpp
 * @brief Definition of the Mesh structure and mesh-related functions.
 *
 * This header provides the Mesh data structure, which stores nodes, elements,
 * and faces of a computational mesh. It also declares functions to read
 * the mesh from a file and initialize boundary flags based on input settings.
 *
 * @author Alessio Improta
 */

#pragma once

#include <array>
#include <fstream>
#include <string>
#include <vector>

#include <eulercpp/mesh/nodes.hpp>
#include <eulercpp/mesh/elements.hpp>
#include <eulercpp/mesh/faces.hpp>
#include <eulercpp/mesh/distances.hpp>
#include <eulercpp/mesh/normals.hpp>

namespace eulercpp {

struct Simulation;

/**
 * @brief Structure representing the computational mesh.
 *
 * Stores the nodes, elements, and faces of the mesh along with
 * counts of nodes, elements, faces, and boundary faces.
 */
struct Mesh {
    int n_nodes = 0;                /**< Total number of nodes. */
    int n_elements = 0;             /**< Total number of elements. */
    int n_faces = 0;                /**< Total number of faces. */
    int n_boundaries = 0;           /**< Number of boundary faces. */

    std::vector<Node> nodes;        /**< Container of all nodes. */
    std::vector<Element> elements;  /**< Container of all elements. */
    std::vector<Face> faces;        /**< Container of all faces. */

    /**
     * @brief Initializes boundary flags for faces based on input settings.
     *
     * Iterates through all faces and sets the `flag` of each face if
     * its centroid lies within the boundary box defined in the input.
     *
     * @param input Input structure containing boundary definitions.
     */
    void init_boundaries(const Input& input);
};

/**
 * @brief Reads and processes the computational mesh from a file.
 *
 * This function opens the specified mesh file and sequentially calls
 * the necessary functions to read nodes, read elements, compute element
 * and face properties, face normals, and distances.
 *
 * @param sim The simulation object containing mesh and input information.
 * @throws std::invalid_argument If the mesh file cannot be opened.
 */
void read_mesh(Simulation& sim);

} // namespace eulercpp
