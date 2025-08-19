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
 * @file faces.cpp
 * @brief Implements face computation and connectivity in the mesh.
 *
 * This file implements functions that compute geometric properties of
 * faces (area, centroid, tangents) and establish connectivity between
 * elements via shared faces. It also identifies boundary faces and sets
 * opposite face indices for neighboring elements.
 *
 * @author Alessio Improta
 */

#include <iostream>
#include <omp.h>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include <eulercpp/mesh/mesh.hpp>
#include <eulercpp/mesh/faces.hpp>
#include <eulercpp/mesh/elements.hpp>
#include <eulercpp/mesh/shapes.hpp>
#include <eulercpp/math/vectors.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp {

static const int hexa_index[24] = {
    1, 2, 3, 4,
    1, 5, 6, 2,
    1, 4, 8, 5,
    2, 6, 7, 3,
    3, 7, 8, 4,
    5, 8, 7, 6
};

static const int prism_index[18] = {
    1, 4, 6, 3,
    2, 3, 6, 5,
    1, 2, 5, 4,
    3, 2, 1,
    4, 5, 6
};

static const int pyramid_index[16] = {
    4, 3, 2, 1,
    1, 2, 5,
    2, 3, 5,
    3, 4, 5,
    4, 1, 5
};

/**
 * @brief Helper struct for hashing face node vectors in a map.
 *
 * Used to identify unique faces by their nodes, regardless of node
 * ordering.
 */
struct FaceKeyHash {
    std::size_t operator()(const std::vector<int>& key) const {
        std::size_t h = 0;
        for (int node : key) {
            h ^= std::hash<int>()(node) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
};

/**
 * @brief Computes neighbor and opposite face relationships for all faces.
 *
 * Iterates through all faces, identifying shared faces between elements
 * and setting neighbor and opposite indices. Also updates element neighbor
 * lists.
 *
 * @param mesh Reference to the Mesh structure.
 */
void compute_face_connectivity(Mesh& mesh) {
    std::unordered_map<std::vector<int>, int, FaceKeyHash> face_map;

    for (int i = 0; i < mesh.n_faces; ++i) {
        Face& face = mesh.faces[i];

        std::vector<int> key = face.nodes;
        std::sort(key.begin(), key.end());

        auto it = face_map.find(key);
        if (it == face_map.end()) {
            face_map[key] = i;
        } else {
            int other_index = it->second;
            Face& other = mesh.faces[other_index];

            face.neighbor = other.owner;
            other.neighbor = face.owner;

            face.opposite = other.id;
            other.opposite = face.id;

            face_map.erase(it);
        }
    }

    Logger::debug() << "Assigning element neighbors...";
    #pragma omp parallel for
    for (int i = 0; i < mesh.n_elements; ++i) {
        Element& elm = mesh.elements[i];
        int n_f = elm.n_faces;
        elm.neighbors.resize(n_f);
        for (int f = 0; f < n_f; ++f) {
            Face& face = mesh.faces[elm.faces[f]];
            elm.neighbors[f] = face.neighbor;
        }
    }
}

/**
 * @brief Computes the properties and connectivity of faces in the mesh.
 *
 * This function calculates face areas, centroids, and element neighbors.
 * It identifies boundary faces and sets up opposite face relationships
 * between neighboring elements.
 *
 * @param mesh Reference to the Mesh structure containing faces and elements.
 */
void compute_faces(Mesh& mesh) {
    Logger::debug() << "Counting faces...";
    for (const auto& elem : mesh.elements) {
        mesh.n_faces += elem.n_faces;
    }

    mesh.faces.resize(mesh.n_faces);

    Logger::debug() << "Computing face properties...";
    int id = 0;
    #pragma omp parallel for
    for (int i = 0; i < mesh.n_elements; ++i) {
        Element& elem = mesh.elements[i];
        elem.faces.resize(elem.n_faces);

        for (int f = 0; f < elem.n_faces; ++f) {
            int local_id;
            #pragma omp atomic capture
            local_id = id++;
            elem.faces[f] = local_id;
            Face& face = mesh.faces[local_id];
            face.id = local_id;
            face.owner = i;

            switch (elem.type) {
                case ElementType::POINT:
                    break;

                case ElementType::LINEAR:
                    face.n_nodes = 1;
                    face.nodes.resize(face.n_nodes);
                    face.nodes[0] = elem.nodes[f];
                    face.centroid = mesh.nodes[face.nodes[0]].position;
                    face.area = 1.0;
                    break;

                case ElementType::TRIA:
                case ElementType::QUAD:
                case ElementType::POLYGON:
                {
                    face.n_nodes = 2;
                    face.nodes.resize(face.n_nodes);
                    face.nodes[0] = elem.nodes[f];
                    face.nodes[1] = elem.nodes[(f+1) % elem.n_nodes];

                    math::Vector3D n1 = mesh.nodes[face.nodes[0]].position;
                    math::Vector3D n2 = mesh.nodes[face.nodes[1]].position;

                    face.centroid = math::mid_point(n1, n2);
                    face.area = math::distance(n1, n2);
                    break;
                }

                case ElementType::TETRA:
                {
                    face.n_nodes = 3;
                    face.nodes.resize(face.n_nodes);
                    for (int j = 0; j < face.n_nodes; ++j) {
                        face.nodes[j] = elem.nodes[(f + j) % elem.n_nodes];
                    }

                    std::vector<math::Vector3D> n(face.n_nodes);
                    for (int k = 0; k < face.n_nodes; ++k) {
                        n[k] = mesh.nodes[face.nodes[k]].position;
                    }

                    face.centroid = tria_centroid(n[0], n[1], n[2]);
                    face.area = math::norm(tria_vector(n[0], n[1], n[2]));
                    break;
                }

                case ElementType::HEXA:
                {
                    face.n_nodes = 4;
                    face.nodes.resize(face.n_nodes);
                    for (int j = 0; j < face.n_nodes; ++j) {
                        face.nodes[j] = elem.nodes[hexa_index[f*4+j]-1];
                    }

                    std::vector<math::Vector3D> n(face.n_nodes);
                    for (int k = 0; k < face.n_nodes; ++k) {
                        n[k] = mesh.nodes[face.nodes[k]].position;
                    }

                    auto [centroid, area, l] = polygon_properties(n);
                    face.centroid = centroid;
                    face.area = area;
                    break;
                }

                case ElementType::PRISM:
                {
                    if (f < 3) {
                        face.n_nodes = 4;
                        face.nodes.resize(face.n_nodes);
                        for (int j = 0; j < face.n_nodes; ++j) {
                            face.nodes[j] =
                                elem.nodes[prism_index[f*4+j]-1];
                        }
                    } else {
                        face.n_nodes = 3;
                        face.nodes.resize(face.n_nodes);
                        for (int j = 0; j < face.n_nodes; ++j) {
                            face.nodes[j] =
                                elem.nodes[prism_index[12+3*(f-3)+j]-1];
                        }
                    }

                    std::vector<math::Vector3D> n(face.n_nodes);
                    for (int k = 0; k < face.n_nodes; ++k) {
                        n[k] = mesh.nodes[face.nodes[k]].position;
                    }

                    auto [centroid, area, l] = polygon_properties(n);
                    face.centroid = centroid;
                    face.area = area;
                    break;
                }

                case ElementType::PYRAMID:
                {
                    if (f < 1) {
                        face.n_nodes = 4;
                        face.nodes.resize(face.n_nodes);
                        for (int j = 0; j < face.n_nodes; ++j) {
                            face.nodes[j] = elem.nodes[pyramid_index[j]-1];
                        }
                    } else {
                        face.n_nodes = 3;
                        face.nodes.resize(face.n_nodes);
                        for (int j = 0; j < face.n_nodes; ++j) {
                            face.nodes[j] = 
                                elem.nodes[pyramid_index[4+3*(f-1)+j]-1];
                        }
                    }

                    std::vector<math::Vector3D> n(face.n_nodes);
                    for (int k = 0; k < face.n_nodes; ++k) {
                        n[k] = mesh.nodes[face.nodes[k]].position;
                    }

                    auto [centroid, area, l] = polygon_properties(n);
                    face.centroid = centroid;
                    face.area = area;
                    break;
                }

                case ElementType::POLYHEDRON:
                {
                    int offset = 0;
                    for (int ff = 0; ff < f; ++ff)
                        offset += elem.nodes[offset] + 1;

                    face.n_nodes = elem.nodes[offset];
                    face.nodes.resize(face.n_nodes);

                    std::vector<math::Vector3D> n(face.n_nodes);
                    for (int k = 0; k < face.n_nodes; ++k) {
                        face.nodes[k] = elem.nodes[offset + 1 + k];
                        n[k] = mesh.nodes[face.nodes[k]].position;
                    }

                    auto [centroid, area, _] = polygon_properties(n);
                    face.centroid = centroid;
                    face.area = area;
                    break;
                }
            }
        }
    }

    Logger::info() << "Loaded " << mesh.n_faces << " faces.";

    Logger::debug() << "Computing face connectivity...";
    compute_face_connectivity(mesh);
}

/**
 * @brief Assign boundary conditions.
 *
 * @param mesh Reference to the Mesh structure.
 * @param input Reference to the simulation Input structure.
 */
void assign_boundaries(Mesh& mesh, Input& input) {
    Logger::debug() << "Counting boundary faces...";
    mesh.n_boundaries = 0;
    for (const auto& face : mesh.faces) {
        if (face.neighbor == -1) {
            mesh.n_boundaries++;
        }
    }
    Logger::info() << "Found " << mesh.n_boundaries << " boundary faces.";

    Logger::debug() << "Assigning boundary conditions...";
    mesh.init_boundaries(input);

    std::unordered_map<std::vector<int>, int, FaceKeyHash> face_map;

    for (int f = 0; f < mesh.n_faces; ++f) {
        std::vector<int> key = mesh.faces[f].nodes;
        std::sort(key.begin(), key.end());
        face_map[key] = f;
    }

    for (int i = 0; i < mesh.n_elements; ++i) {
        const auto& elem = mesh.elements[i];
        if (!elem.boundary) continue;

        std::vector<int> key = elem.nodes;
        std::sort(key.begin(), key.end());

        auto it = face_map.find(key);
        if (it != face_map.end()) {
            int f = it->second;
            mesh.faces[f].flag = elem.tags[0];
        }
    }

    // Erase boundary elements
    mesh.elements.erase(
        std::remove_if(mesh.elements.begin(), mesh.elements.end(),
            [](const Element& s) { return s.boundary; }),
        mesh.elements.end()
    );

    // Update number of elements
    mesh.n_elements = mesh.elements.size();
}

} // namespace eulercpp
