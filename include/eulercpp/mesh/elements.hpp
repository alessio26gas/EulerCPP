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
 * @file elements.hpp
 * @brief Definitions for mesh elements (cells) in EulerCPP.
 *
 * This file defines the Element struct representing computational
 * mesh elements, the ElementType enum, and functions for reading
 * and computing element properties.
 *
 * @author Alessio Improta
 */

#pragma once

#include <array>
#include <fstream>
#include <string>
#include <vector>

namespace eulercpp {

struct Input;
struct Mesh;

/**
 * @enum ElementType
 * @brief Types of finite elements used in the computational mesh.
 *
 * Enumerates all possible element shapes that can be read from the mesh
 * file and used in the CFD simulation.
 */
enum class ElementType {
    POINT,          /**< Point element */
    LINEAR,         /**< Linear element */
    TRIA,           /**< Triangular element */
    QUAD,           /**< Quadrilateral element */
    TETRA,          /**< Tetrahedral element */
    HEXA,           /**< Hexahedral element */
    PRISM,          /**< Triangular prism element */
    PYRAMID,        /**< Pyramid element */
    POLYGON,        /**< Polygonal element */
    POLYHEDRON,     /**< Polyhedral element */
};

/**
 * @struct Element
 * @brief Represents a single element (cell) in the computational mesh.
 *
 * Each Element contains identifiers, connectivity, geometry, and
 * precomputed properties used during CFD simulation. It stores node
 * indices, face indices, neighbor relationships, volume, centroid,
 * and distances for numerical schemes.
 *
 * @note The element type must be set according to the ElementType enum.
 */
struct Element {
    int id = 0; /**< Unique identifier of the element. */
    int dimension = 0; /**< Element spatial dimension. */
    ElementType type = ElementType::POINT; /**< Type of the element. */
    std::vector<int> tags; /**< Element tags. */

    int n_nodes = 0; /**< Number of nodes defining the element. */
    std::vector<int> nodes; /**< Indices of nodes forming the element. */

    int n_faces = 0;            /**< Number of faces forming the element. */
    std::vector<int> faces;     /**< Indices of faces forming the element. */
    std::vector<int> neighbors; /**< Indices of neighbor elements. */

    double volume = 0.0;                /**< Volume of the element. */
    std::array<double, 3> centroid;     /**< Element centroid. */

    std::vector<std::array<double, 3>> d;   /**< Distance to neighbor cells. */
    std::vector<std::array<double, 3>> df;  /**< Distance to face centroids. */

    std::vector<std::array<double, 3>> w; /**< Reconstruction weights. */
    std::array<std::array<double, 3>, 3> S = {0.0}; /**< Inverse S matrix. */

    bool boundary = false;    /**< True if the element is a boundary face. */
};

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
void read_elements(std::ifstream& file, Mesh& mesh);

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
void compute_elements(Mesh& mesh, Input& input);

} // namespace eulercpp
