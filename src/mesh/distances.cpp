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
 * @file distances.cpp
 * @brief Implements computation of element-to-face and
 *        element-to-element distances.
 *
 * This file provides the implementation of distance vector
 * calculations. For each element, vectors from centroids to faces
 * (`df`) and to neighbors (`d`) are computed. These vectors are used
 * to construct reconstruction weights and the inverse reconstruction
 * matrix `S`.
 *
 * @author Alessio Improta
 */

#include <omp.h>

#include <eulercpp/mesh/mesh.hpp>
#include <eulercpp/mesh/distances.hpp>
#include <eulercpp/math/vectors.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp {

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
void compute_distances(Mesh& mesh, const int dimension) {
    const int dim_ = dimension == 3 ? 3 : dimension == 0 ? 1 : 2;

    Logger::debug() << "Computing distances for each element...";
    #pragma omp parallel for
    for (int i = 0; i < mesh.n_elements; ++i) {
        Element& elem = mesh.elements[i];
        int nf = elem.n_faces;

        elem.d.resize(nf);
        elem.df.resize(nf);
        elem.w.resize(nf);

        std::array<std::array<double, 3>, 3> S = {0.0};
        for (int f = 0; f < nf; ++f) {
            int fi = elem.faces[f];
            auto& face = mesh.faces[fi];

            for (int dim = 0; dim < 3; ++dim) {
                elem.df[f][dim] = face.centroid[dim] - elem.centroid[dim];
            }

            int j = elem.neighbors[f];
            if (j < 0) continue;

            for (int dim = 0; dim < 3; ++dim) {
                elem.d[f][dim] = mesh.elements[j].centroid[dim]
                                 - elem.centroid[dim];
            }

            double w = 1.0 / math::dot_product(elem.d[f], elem.d[f]);
            for (int dim = 0; dim < 3; ++dim) {
                elem.w[f][dim] = w * elem.d[f][dim];
                for (int dim_j = 0; dim_j < 3; ++dim_j) {
                    S[dim][dim_j] += elem.w[f][dim] * elem.d[f][dim_j];
                }
            }
        }

        double det;
        if (dim_ == 3) {
            det = S[0][0] * (S[1][1] * S[2][2] - S[1][2] * S[2][1]) +
                  S[0][1] * (S[1][2] * S[2][0] - S[1][0] * S[2][2]) +
                  S[0][2] * (S[1][0] * S[2][1] - S[1][1] * S[2][0]);
            double invdet = 1.0 / det;

            elem.S[0][0] =  (S[1][1] * S[2][2] - S[1][2] * S[2][1]) * invdet;
            elem.S[0][1] = -(S[0][1] * S[2][2] - S[0][2] * S[2][1]) * invdet;
            elem.S[0][2] =  (S[0][1] * S[1][2] - S[0][2] * S[1][1]) * invdet;

            elem.S[1][0] = -(S[1][0] * S[2][2] - S[1][2] * S[2][0]) * invdet;
            elem.S[1][1] =  (S[0][0] * S[2][2] - S[0][2] * S[2][0]) * invdet;
            elem.S[1][2] = -(S[0][0] * S[1][2] - S[0][2] * S[1][0]) * invdet;

            elem.S[2][0] =  (S[1][0] * S[2][1] - S[1][1] * S[2][0]) * invdet;
            elem.S[2][1] = -(S[0][0] * S[2][1] - S[0][1] * S[2][0]) * invdet;
            elem.S[2][2] =  (S[0][0] * S[1][1] - S[0][1] * S[1][0]) * invdet;

        } else if (dim_ == 2) {
            det = S[0][0] * S[1][1] - S[0][1] * S[1][0];
            double invdet = 1.0 / det;

            elem.S[0][0] =  S[1][1] * invdet;
            elem.S[0][1] = -S[0][1] * invdet;
            elem.S[1][0] = -S[1][0] * invdet;
            elem.S[1][1] =  S[0][0] * invdet;

            elem.S[2][2] = 1.0;
        } else {
            det = S[0][0];
            double invdet = 1.0 / det;

            elem.S[0][0] = invdet;

            elem.S[1][1] = 1.0;
            elem.S[2][2] = 1.0;
        }
    }
}

} // namespace eulercpp
