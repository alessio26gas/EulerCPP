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
 * @file elements.cpp
 * @brief Implements reading and computation of mesh elements.
 *
 * This file provides functions to read the `$Elements` section of a mesh file
 * and to compute geometric properties such as volume and centroid.
 *
 * Elements are the primary volumetric units used in CFD simulations. Each
 * element contains references to its nodes, faces, and neighbors.
 *
 * @see mesh.hpp, elements.hpp, input.hpp, logger.hpp
 * @author Alessio Improta
 */

#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include <eulercpp/input/input.hpp>
#include <eulercpp/mesh/mesh.hpp>
#include <eulercpp/mesh/elements.hpp>
#include <eulercpp/mesh/shapes.hpp>
#include <eulercpp/math/vectors.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp {

/**
 * @brief Parses an integer from a C-string pointer.
 *
 * @param p Pointer to the C-string to parse.
 * @param out Reference to an integer where the parsed value will be stored.
 * @return Pointer to the next character after the parsed integer.
 *
 * @note The function uses `std::strtol` internally and assumes valid input.
 */
inline const char* parse_int(const char* p, int& out) {
    out = std::strtol(p, const_cast<char**>(&p), 10);
    return p;
}

/**
 * @brief Reads the element data from a mesh file into the mesh structure.
 *
 * This function searches for the `$Elements` section in the mesh file and
 * reads element IDs, types, and node connectivity. It populates the mesh's
 * `elements` vector.
 *
 * @param file Reference to an input file stream positioned at the beginning
 *             of the mesh file.
 * @param mesh Reference to the Mesh structure.
 *
 * @throws std::runtime_error If the `$Elements` section is missing, the number
 *         of elements is invalid, or if element data cannot be read correctly.
 *
 * @note The file format must include one element per line, specifying the
 *       element type and the indices of its nodes.
 */
void read_elements(std::ifstream& file, Mesh& mesh) {
    Logger::debug() << "Reading elements...";

    const std::unordered_set<int> valid2D{2, 3, 8};
    const std::unordered_set<int> valid3D{4, 5, 6, 7, 9};

    std::string line;
    int counts[10] = {0};

    while (std::getline(file, line)) {
        if (line.rfind("$Elements", 0) == 0) {
            if (!std::getline(file, line)) {
                throw std::runtime_error("Could not read number of elements.");
            }

            mesh.n_elements = std::stoi(line);
            if (mesh.n_elements <= 0) {
                throw std::runtime_error("No elements found.");
            }
            mesh.elements.resize(mesh.n_elements);

            for (int i = 0; i < mesh.n_elements; ++i) {
                if (!std::getline(file, line)) {
                    throw std::runtime_error("Unexpected end of file.");
                }

                const char* p = line.c_str();
                int id, type, n_tags;
                p = parse_int(p, id);
                p = parse_int(p, type);
                p = parse_int(p, n_tags);

                std::vector<int> tags;
                if (n_tags > 0) {
                    tags.resize(n_tags);
                    for (int t = 0; t < n_tags; ++t) {
                        p = parse_int(p, tags[t]);
                    }
                }

                int n_nodes = 0, n_faces = 0, dim = 0;
                std::vector<int> nodes;

                if (type == static_cast<int>(ElementType::POLYHEDRON)) {
                    dim = 3;
                    p = parse_int(p, n_faces);
                    for (int f = 0; f < n_faces; ++f) {
                        int face_nodes;
                        p = parse_int(p, face_nodes);
                        n_nodes += face_nodes;
                        nodes.push_back(face_nodes);
                        for (int n = 0; n < face_nodes; ++n) {
                            int node_id;
                            p = parse_int(p, node_id);
                            nodes.push_back(node_id - 1);
                        }
                    }
                } else {
                    switch (static_cast<ElementType>(type)) {
                        case ElementType::POINT:
                            n_nodes = 1; n_faces = 0; dim = 0; break;
                        case ElementType::LINEAR:
                            n_nodes = 2; n_faces = 2; dim = 1; break;
                        case ElementType::TRIA:
                            n_nodes = 3; n_faces = 3; dim = 2; break;
                        case ElementType::QUAD:
                            n_nodes = 4; n_faces = 4; dim = 2; break;
                        case ElementType::TETRA:
                            n_nodes = 4; n_faces = 4; dim = 3; break;
                        case ElementType::HEXA:
                            n_nodes = 8; n_faces = 6; dim = 3; break;
                        case ElementType::PRISM:
                            n_nodes = 6; n_faces = 5; dim = 3; break;
                        case ElementType::PYRAMID:
                            n_nodes = 5; n_faces = 5; dim = 3; break;
                        case ElementType::POLYGON:
                            dim = 2;
                            p = parse_int(p, n_faces);
                            n_nodes = n_faces;
                            break;
                        default:
                            throw std::runtime_error(
                                "Unsupported element type: "
                                + std::to_string(type)
                            );
                    }

                    nodes.reserve(n_nodes);
                    for (int n = 0; n < n_nodes; ++n) {
                        int node_id;
                        p = parse_int(p, node_id);
                        nodes.push_back(node_id - 1);
                    }
                }

                Element& elem = mesh.elements[i];
                elem.id = id;
                elem.dimension = dim;
                elem.type = static_cast<ElementType>(type);
                elem.tags = std::move(tags);
                elem.n_nodes = n_nodes;
                elem.n_faces = n_faces;
                elem.nodes = std::move(nodes);

                counts[type]++;
            }

            Logger::info() << "Read " << mesh.n_elements << " elements:";
            const char* names[] = {
                "POINT", "LINEAR", "TRIA", "QUAD", "TETRA",
                "HEXA", "PRISM", "PYRAMID", "POLYGON", "POLYHEDRON"
            };
            for (int i = 0; i < 10; ++i) {
                if (counts[i] > 0) {
                    Logger::info() << " - " << names[i] << ": " << counts[i];
                }
            }
            return;
        }
    }

    throw std::runtime_error("No $Elements section found in mesh file.");
}

/**
 * @brief Computes geometric properties of mesh elements.
 *
 * For each element in the mesh, this function computes:
 * - Volume
 * - Centroid
 *
 * These properties are essential for flux computation and numerical schemes
 * in CFD simulations.
 *
 * @param mesh Reference to the Mesh containing elements and nodes.
 * @param input Reference to the simulation Input structure.
 */
void compute_elements(Mesh& mesh, Input& input) {
    Logger::debug() << "Computing element properties...";

    std::vector<double> volumes(mesh.n_elements);

    const int dim_ = input.physics.dimension;
    const int dimension = dim_ == 3 ? 3 : dim_ == 0 ? 1 : 2;

    #pragma omp parallel for
    for (int i = 0; i < mesh.n_elements; ++i) {
        Element& elem = mesh.elements[i];
        if (elem.dimension > dimension) {
            throw std::runtime_error("Invalid element dimension.");
        }
        if (elem.dimension < dimension-1) {
            throw std::runtime_error("Invalid element dimension.");
        }
        if (elem.dimension == dimension-1) {
            if (elem.tags.size() == 0) {
                throw std::runtime_error("Invalid element dimension.");
            }
            elem.boundary = true;
            elem.n_faces = 0;
            continue;
        }

        std::vector<std::array<double, 3>> n(elem.n_nodes);
        for (int j = 0; j < elem.n_nodes; ++j) {
            n[j] = mesh.nodes[elem.nodes[j]].position;
        }

        switch (elem.type) {
        case ElementType::POINT:
            elem.centroid = n[0];
            elem.volume = 1.0;
            break;
        
        case ElementType::LINEAR:
            elem.centroid = math::mid_point(n[0], n[1]);
            elem.volume = math::distance(n[0], n[1]);
            break;

        case ElementType::TRIA:
            elem.centroid = tria_centroid(n[0], n[1], n[2]);
            elem.volume = math::norm(tria_vector(n[0], n[1], n[2]));
            break;

        case ElementType::QUAD:
        case ElementType::POLYGON:
        {
            auto [centroid, area, l] = polygon_properties(n);
            elem.centroid = centroid;
            elem.volume = area;
            break;
        }

        case ElementType::TETRA:
            elem.centroid = tetra_centroid(n[0], n[1], n[2], n[3]);
            elem.volume = tetra_volume(n[0], n[1], n[2], n[3]);
            break;

        case ElementType::HEXA:
        {
            auto [centroid, volume, l] = hexa_properties(n);
            elem.centroid = centroid;
            elem.volume = volume;
            break;
        }

        case ElementType::PRISM:
        {
            auto [centroid, volume, l] = prism_properties(n);
            elem.centroid = centroid;
            elem.volume = volume;
            break;
        }

        case ElementType::PYRAMID:
        {
            auto [centroid, volume, l] = pyramid_properties(n);
            elem.centroid = centroid;
            elem.volume = volume;
            break;
        }

        case ElementType::POLYHEDRON:
        {
            auto [centroid, volume, l] = polyhedron_properties(
                elem.n_faces, elem.nodes, mesh
            );
            elem.centroid = centroid;
            elem.volume = volume;
            break;
        }
        }
        volumes[i] = elem.volume;
    }

    double min_volume = volumes[0];
    double max_volume = volumes[0];
    for (size_t i = 1; i < volumes.size(); ++i) {
        if (mesh.elements[i].boundary) continue;
        if (volumes[i] < min_volume) min_volume = volumes[i];
        if (volumes[i] > max_volume) max_volume = volumes[i];
    }
    Logger::info() << "Minimum element volume: " << min_volume;
    Logger::info() << "Maximum element volume: " << max_volume;
    if (min_volume < input.mesh.min_volume) {
        std::ostringstream oss;
        oss << "Minimum cell volume is too small (" << min_volume << ")";
        throw std::runtime_error(oss.str());
    }
}

} // namespace eulercpp
