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
 * @file normals.cpp
 * @brief Implements computation of face normals and tangent vectors.
 *
 * This file provides the implementation of algorithms to compute
 * geometric information for mesh faces. For each face, an outward
 * pointing normal vector is calculated based on the element type
 * and topology. Tangent vectors are then constructed to form an
 * orthonormal local basis on the face.
 *
 * @author Alessio Improta
 */

#include <array>
#include <cmath>
#include <iostream>
#include <omp.h>

#include <eulercpp/mesh/mesh.hpp>
#include <eulercpp/mesh/normals.hpp>
#include <eulercpp/math/vectors.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp {

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
void compute_normals(Mesh& mesh) {
    /// Compute face normals
    Logger::debug() << "Computing face normals...";
    #pragma omp parallel for
    for (int i = 0; i < mesh.n_elements; ++i) {
        Element& element = mesh.elements[i];
        math::Vector3D c = element.centroid;
        int nf = element.n_faces;

        switch (element.type)
        {
        case ElementType::POINT:
            break;

        case ElementType::LINEAR:
            for (int f = 0; f < nf; ++f) {
                Face& face = mesh.faces[element.faces[f]];
                math::Vector3D cf = face.centroid;
                math::Vector3D n = {cf[0]-c[0], cf[1]-c[1], cf[2]-c[2]};
                math::normalize(n);
                face.normal = n;
            }
            break;

        case ElementType::TRIA:
        case ElementType::QUAD:
        case ElementType::POLYGON:
            for (int f = 0; f < nf; ++f) {
                Face& face = mesh.faces[element.faces[f]];

                math::Vector3D t = {
                    mesh.nodes[face.nodes[1]].position[0] -
                    mesh.nodes[face.nodes[0]].position[0],
                    mesh.nodes[face.nodes[1]].position[1] -
                    mesh.nodes[face.nodes[0]].position[1],
                    mesh.nodes[face.nodes[1]].position[2] -
                    mesh.nodes[face.nodes[0]].position[2]
                };

                math::Vector3D cf = face.centroid;
                math::Vector3D b = {c[0]-cf[0], c[1]-cf[1], c[2]-cf[2]};

                math::Vector3D n = math::cross_product(
                    math::cross_product(b, t), t
                );

                if (math::dot_product(b, n) > 0) {
                    n[0] = -n[0];
                    n[1] = -n[1];
                    n[2] = -n[2];
                }
                math::normalize(n);

                face.normal = n;
            }
            break;
            
        case ElementType::TETRA:
        case ElementType::HEXA:
        case ElementType::PRISM:
        case ElementType::PYRAMID:
        case ElementType::POLYHEDRON:
            for (int f = 0; f < nf; ++f) {
                Face& face = mesh.faces[element.faces[f]];
                int face_nodes = face.n_nodes;

                math::Vector3D H = {0.0, 0.0, 0.0};
                for (int d = 0; d < 3; ++d) {
                    for (int j = 0; j < face_nodes; ++j) {
                        H[d] += mesh.nodes[face.nodes[j]].position[d];
                    }
                    H[d] /= face_nodes;
                }

                math::Vector3D n = {0.0, 0.0, 0.0};

                math::Vector3D v1, v2, res;
                for (int j = 0; j < face_nodes; ++j) {
                    int k = (j + 1) % face_nodes;
                    for (int d = 0; d < 3; ++d) {
                        v1[d] = mesh.nodes[face.nodes[j]].position[d] - H[d];
                        v2[d] = mesh.nodes[face.nodes[k]].position[d] - H[d];                     
                    }
                    res = math::cross_product(v1, v2);
                    for (int d = 0; d < 3; ++d) {
                        n[d] += res[d];
                    }
                }

                math::Vector3D cf = face.centroid;
                math::Vector3D b = {c[0]-cf[0], c[1]-cf[1], c[2]-cf[2]};
                if (math::dot_product(b, n) > 0) {
                    n[0] = -n[0];
                    n[1] = -n[1];
                    n[2] = -n[2];
                }
                math::normalize(n);

                face.normal = n;
            }
            break;
        }
    }

    Logger::debug() << "Computing face tangents...";
    /// Compute face tangents
    #pragma omp parallel for
    for (int i = 0; i < mesh.n_faces; ++i) {
        Face& face = mesh.faces[i];

        const math::Vector3D& n = face.normal;

        int delta = std::abs(n[2]) < 0.5 ? 0 : 1;
        std::array<int, 3> a = {0, delta, 1 - delta};

        double t1d = std::sqrt(n[0]*n[0] + a[2]*n[1]*n[1] + a[1]*n[2]*n[2]);

        face.t1[0] = (a[1]*n[2] - a[2]*n[1]) / t1d;
        face.t1[1] = a[2]*n[0] / t1d;
        face.t1[2] = -a[1]*n[0] / t1d;

        face.t2 = math::cross_product(n, face.t1);
    }
}

} // namespace eulercpp
