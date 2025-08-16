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
 * @file distances.hpp
 * @brief Declares functions for computing element-to-face and
 *        element-to-element distances.
 *
 * This file contains the declaration of distance computation routines.
 * For each element, vectors to face centroids and neighboring centroids
 * are computed and stored, along with reconstruction weights.
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
 * @brief Computes element-to-face and element-to-element distances.
 *
 * For each element, this function calculates:
 * - **df**: Vector from element centroid to face centroid.
 * - **d** : Vector from element centroid to neighbor centroid.
 * - **w** : Reconstruction weights based on normalized `d` vectors.
 * - **S** : Inverse of the reconstruction matrix used in gradient
 *           evaluation.
 *
 * The computation adapts to the specified dimension:
 * - **3D**: Full 3x3 reconstruction matrix.
 * - **2D**: 2x2 reconstruction matrix embedded in 3x3.
 * - **1D**: Single scalar reconstruction weight.
 *
 * @param mesh Reference to the mesh containing elements and faces.
 * @param dimension Input dimension cods as defined in load_physics.hpp.
 */
void compute_distances(Mesh& mesh, const int dimension);

} // namespace eulercpp
