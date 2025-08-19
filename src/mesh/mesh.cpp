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
 * @file mesh.cpp
 * @brief Implementation of mesh reading and initialization functions.
 *
 * This file contains functions to load and process the computational 
 * mesh from a file. It includes:
 * - Reading nodes and elements from a mesh file.
 * - Computing element and face properties.
 * - Computing face normals and distances.
 * - Initializing boundary faces based on input definitions.
 *
 * The functions operate on the `Mesh` structure within a `Simulation`.
 * Performance-critical loops use OpenMP for parallelization.
 *
 * @author Alessio Improta
 */

#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>

#include <eulercpp/input/input.hpp>
#include <eulercpp/mesh/mesh.hpp>
#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/output/logger.hpp>
#include <eulercpp/math/time_utils.hpp>
#include <eulercpp/math/vectors.hpp>

namespace eulercpp {

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
void read_mesh(Simulation& sim) {
    clock_t start = clock();

    Input& input = sim.input;
    Mesh& mesh = sim.mesh;

    /// Open mesh file
    const std::string filename = input.mesh.mesh_file;
    Logger::debug() << "Opening mesh file " << filename << "...";
    std::ifstream file(filename);
    if (!file) {
        throw std::invalid_argument("Cannot open mesh file " + filename);
    }
    Logger::info() << "Reading mesh from " << filename;

    /// Read nodes
    read_nodes(file, mesh);

    /// Read elements
    read_elements(file, mesh);

    /// Close mesh file
    file.close();

    /// Compute elements properties
    compute_elements(mesh, input);

    /// Compute face properties
    compute_faces(mesh);

    /// Assign boundary conditions
    assign_boundaries(mesh, input);

    /// Compute face normals
    compute_normals(mesh);

    /// Compute distances
    compute_distances(mesh, input.physics.dimension);

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    Logger::success() << "Mesh loaded. (" 
                      << math::format_duration(elapsed) << ")";
}

/**
 * @brief Initializes boundary flags for faces based on input settings.
 *
 * Iterates through all faces and sets the `flag` of each face if
 * its centroid lies within the boundary box defined in the input.
 *
 * @param input Input structure containing boundary definitions.
 */
void Mesh::init_boundaries(const Input& input) {
    constexpr double eps = 1e-12;

    #pragma omp parallel for
    for (int i = 0; i < n_faces; ++i) {
        const auto& c = faces[i].centroid;
        const double x = c[0], y = c[1], z = c[2];

        for (int b = 0; b < input.bc.n_boundaries; ++b) {
            const auto& bc = input.bc.boundaries[b];
            if (x < bc.xmax + eps && x > bc.xmin - eps &&
                y < bc.ymax + eps && y > bc.ymin - eps &&
                z < bc.zmax + eps && z > bc.zmin - eps &&
                math::distance(c, bc.center) < bc.radius + eps) {
                faces[i].flag = b;
            }
        }
    }
}

} // namespace eulercpp
