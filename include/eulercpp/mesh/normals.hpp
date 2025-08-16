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
 * @file normals.hpp
 * @brief Declares functions for computing face normals and tangents.
 *
 * This file provides the declaration of functions used to calculate
 * normal and tangent vectors for mesh faces. Normals are oriented
 * consistently outward from element centroids, while tangents are
 * constructed to form an orthonormal basis.
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
 * @brief Computes face normal and tangent vectors for all mesh faces.
 *
 * This function performs two main steps:
 * 
 * 1. **Normal Vector Computation**: For each face of each element, compute 
 *    a unit-length normal vector pointing outward from the element's 
 *    centroid. The method varies depending on the element type:
 * 
 *    - **1D linear elements (LINEAR)**: Normals are computed from the 
 *      direction between element centroid and face centroid.
 * 
 *    - **2D surface elements (TRIA, QUAD, POLYGON)**: Normals are 
 *      orthogonal to the face, computed using cross products of edge 
 *      vectors.
 * 
 *    - **3D volume elements (TETRA, HEXA, PRISM, PYRAMID, POLYHEDRON)**: 
 *      Normals are obtained by triangulating the face around a temporary 
 *      centroid and summing the cross products.
 * 
 * 2. **Tangent Vector Computation**: After normals are computed, each face
 *    is assigned two orthonormal tangent vectors `t1` and `t2` via a 
 *    deterministic projection method to ensure consistency.
 *
 * @param mesh Reference to the mesh containing elements, faces, and nodes.
 */
void compute_normals(Mesh& mesh);

} // namespace eulercpp
