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
 * @file reconstruction.cpp
 * @brief Implements reconstruction schemes for numerical flux computation.
 *
 * Contains internal implementations of:
 * - Piecewise constant reconstruction
 * - MUSCL reconstruction with slope limiting
 *
 * Provides public interfaces for initializing and invoking these schemes.
 * MUSCL reconstruction uses a limiter function to maintain monotonicity.
 *
 * @see reconstruction.hpp
 * @author Alessio Improta
 */

#include <array>
#include <omp.h>

#include <eulercpp/math/reconstruction.hpp>
#include <eulercpp/math/vectors.hpp>
#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp::math {

/** @brief Internal pointer to the current reconstruction scheme */
static ReconstructionScheme reconstruction_scheme = nullptr;

/** @brief Internal pointer to the limiter function for MUSCL */
static LimiterFunction limiter = nullptr;

/**
 * @brief Piecewise constant reconstruction.
 *
 * Sets face values equal to the owner cell values.
 *
 * @param sim Simulation object to update.
 */
static inline void constant_reconstruction(eulercpp::Simulation& sim) {
    const Input& input = sim.input;
    const Mesh& mesh = sim.mesh;
    Fields& fields = sim.fields;

    #pragma omp parallel for
    for (int f = 0; f < mesh.n_faces; ++f) {
        int o = mesh.faces[f].owner;
        for (int v = 0; v < 5; ++v) {
            fields.Wf(f, v) = fields.W(o, v);
        }
    }
}

/**
 * @brief MUSCL reconstruction with slope limiting.
 *
 * Reconstructs face values using cell-centered gradients and a limiter
 * function to maintain monotonicity. The limiter ensures no spurious
 * oscillations occur near discontinuities.
 *
 * @param sim Simulation object to update.
 */
static inline void muscl_reconstruction(eulercpp::Simulation& sim) {
    const Input& input = sim.input;
    const Mesh& mesh = sim.mesh;
    Fields& fields = sim.fields;

    const int n_elements = mesh.n_elements;
    const int n_var = 5;

    #pragma omp parallel for
    for (int i = 0; i < n_elements; ++i) {
        const auto& elem = mesh.elements[i];
        const auto& df = elem.df;
        const int n_f = elem.n_faces;

        for (int v = 0; v < n_var; ++v) {
            const double W = fields.W(i, v);

            double Wmin = W;
            double Wmax = W;

            for (int f = 0; f < n_f; ++f) {
                const int n = elem.neighbors[f];
                if (n < 0) continue;

                Wmax = std::max(Wmax, fields.W(n, v));
                Wmin = std::min(Wmin, fields.W(n, v));
            }

            const double Dmax = Wmax - W;
            const double Dmin = Wmin - W;

            double alpha = 1.0;

            for (int f = 0; f < n_f; ++f) {
                const double Df = math::dot_product(fields.gradW(i, v), df[f]);

                if ((Df >= 0.0 && Dmax < 1.0e-5) || (Df <= 0.0 && Dmin > -1.0e-5)) {
                    alpha = 0.0;
                    break;
                }

                const double rf = (Df > 0.0) ? Df / Dmax : Df / Dmin;
                alpha = std::min(alpha, limiter(rf));
            }

            for (int f = 0; f < n_f; ++f) {
                const int fi = elem.faces[f];
                fields.Wf(fi, v) = W + alpha * (math::dot_product(fields.gradW(i, v), df[f]));
            }
        }
    }
}

/**
 * @brief Applies the current reconstruction scheme to a simulation.
 *
 * Calls the function pointer set by `init_reconstruction`.
 *
 * @param sim Simulation object to apply reconstruction to.
 */
void reconstruction(eulercpp::Simulation& sim) {
    reconstruction_scheme(sim);
}

/**
 * @brief Initializes the reconstruction scheme.
 *
 * Maps the `Reconstruction` enum to the corresponding internal function.
 *
 * @param scheme Selected reconstruction scheme.
 * @throws std::invalid_argument if the scheme is unknown.
 */
void init_reconstruction(Reconstruction scheme) {
    switch (scheme) {
        case Reconstruction::CONSTANT:  reconstruction_scheme = constant_reconstruction; break;
        case Reconstruction::MUSCL:     reconstruction_scheme = muscl_reconstruction;    break;
        default: throw std::invalid_argument("Unknown reconstruction scheme.");
    }
}

/**
 * @brief Initializes the limiter function for MUSCL reconstruction.
 *
 * Must be called before using MUSCL reconstruction to ensure the limiter
 * function is set.
 *
 * @param type Limiter type.
 */
void init_limiter(Limiter type) {
    limiter = get_limiter(type);
}

} // namespace eulercpp::math
