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
 * @file gradients.cpp
 * @brief Implementation of gradient computation for finite-volume meshes.
 *
 * This file defines the `compute_gradients()` function that calculates
 * spatial gradients of conserved variables (density, momentum, energy)
 * for each element in a mesh. Gradients are computed using neighboring
 * elements and face-weight vectors.
 *
 * OpenMP is used to parallelize the computation over all mesh elements.
 *
 * @see gradients.hpp for the function declaration.
 */

#include <omp.h>

#include <eulercpp/math/gradients.hpp>
#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp::math {

/**
 * @brief Compute gradients of conserved variables in a simulation.
 *
 * Computes the gradients of all variables stored in `sim.fields.W`
 * for every element of the mesh. Gradients are calculated using
 * neighboring elements and face-weight vectors and stored in
 * `sim.fields.gradW`.
 *
 * @param sim Reference to the Simulation containing mesh and fields.
 *
 * @note The computation is parallelized over mesh elements using OpenMP.
 */
void compute_gradients(Simulation& sim) {
    const Mesh& mesh = sim.mesh;
    Fields& fields = sim.fields;

    const int n_elements = mesh.n_elements;
    const int n_var = 5;
    const int dim = fields.dimension();

    #pragma omp parallel for
    for (int i = 0; i < n_elements; ++i) {
        const auto& elem = mesh.elements[i];
        const auto& S = elem.S;
        const auto& w = elem.w;
        const auto& neighbors = elem.neighbors;
        const int n_f = elem.n_faces;
        std::array<double, 3> b;
        std::array<double, 3> g;

        for (int v = 0; v < n_var; ++v) {
            const double W = fields.W(i, v);

            b = {0.0, 0.0, 0.0};
            for (int f = 0; f < n_f; ++f) {
                const int n = neighbors[f];
                if (n < 0) continue;
                const double dW = fields.W(n, v) - W;
                for (int d = 0; d < dim; ++d) b[d] += w[f][d] * dW;
            }

            g = {0.0, 0.0, 0.0};
            for (int d = 0; d < dim; ++d)
                for (int e = 0; e < dim; ++e)
                    g[d] += S[d][e] * b[e];

            fields.gradW(i, v) = g;
        }
    }
}

} // namespace eulercpp::math
