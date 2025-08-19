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
 * @file faces.hpp
 * @brief Defines the Face struct and face-related operations.
 *
 * This file defines the Face struct, which represents a face in the
 * computational mesh. Faces connect two elements and store geometric
 * information such as area, centroid, and tangents. The file also
 * declares functions for computing face properties.
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
 * @struct Face
 * @brief Represents a face in the computational mesh.
 *
 * A face is defined by a set of nodes and connects two elements. 
 * It stores geometric and topological information such as area, 
 * centroid, normal vector, tangents, and element connectivity.
 */
struct Face {
    int id = -1;                    /**< Unique identifier of the face. */
    int flag = -1;                  /**< Flag for boundary specification. */

    int n_nodes = -1;               /**< Number of nodes defining the face. */
    std::vector<int> nodes;         /**< Indices of nodes forming the face. */

    int owner = -1;     /**< Index of the element that owns the face. */
    int neighbor = -1;  /**< Index of the adjacent element. */
    int opposite = -1;  /**< Index of the opposite face. */

    double area;                            /**< Area of the face. */
    std::array<double, 3> centroid = {0.0}; /**< Face centroid. */
    std::array<double, 3> normal = {0.0};   /**< Normal vector of the face. */
    std::array<double, 3> t1 = {0.0};       /**< First tangent vector. */
    std::array<double, 3> t2 = {0.0};       /**< Second tangent vector. */
};

/**
 * @brief Computes the properties and connectivity of faces in the mesh.
 *
 * This function calculates face areas, centroids, and element neighbors.
 * It identifies boundary faces and sets up opposite face relationships
 * between neighboring elements.
 *
 * @param mesh Reference to the Mesh structure containing faces and elements.
 */
void compute_faces(Mesh& mesh);

/**
 * @brief Assign boundary conditions.
 *
 * @param mesh Reference to the Mesh structure.
 * @param input Reference to the simulation Input structure.
 */
void assign_boundaries(Mesh& mesh, Input& input);

} // namespace eulercpp
