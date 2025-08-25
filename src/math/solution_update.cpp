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
 * @file solution_update.cpp
 * @brief Implementation of the solution update for time integration.
 *
 * This source file contains the implementation of advancing the
 * simulation solution for one stage of a multi-stage time integration
 * scheme. It computes the updated conservative variables using fluxes 
 * and source terms stored in the Simulation object.
 *
 * The file also maintains an internal static counter to track the
 * current stage in multi-stage time integration.
 *
 * @see solution_update.hpp
 *
 * @author Alessio Improta
 */

#include <omp.h>
#include <vector>

#include <eulercpp/math/solution_update.hpp>
#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp::math {

/** 
 * @brief Internal counter for multi-stage time integration.
 *
 * This static variable keeps track of the current stage in
 * multi-stage integration schemes.
 */
static int inner_iter = 0;

/**
 * @brief Advances the simulation solution by one time step/stage.
 *
 * This function updates the conservative variables of the simulation
 * based on the numerical fluxes and source terms. It supports multi-stage
 * time integration methods, using an internal counter to select the
 * appropriate stage coefficient.
 *
 * @param sim Reference to the Simulation object containing
 *            mesh, fields, and numerical parameters.
 */
void advance_solution(Simulation& sim) {
    const auto& input = sim.input;
    const auto& mesh = sim.mesh;
    const auto& status = sim.status;
    auto& fields = sim.fields;

    const int n_elements = mesh.n_elements;
    const int n_var = 5;
    double dt = status.dt;

    const auto& a = input.numerical.a;
    #pragma omp parallel for
    for (int i = 0; i < n_elements; ++i) {
        const auto& elem = mesh.elements[i];
        for (int v = 0; v < n_var; ++v) {
            /// Sum flux contributions from all faces
            double dF = 0.0;
            for (int f = 0; f < elem.n_faces; ++f) {
                int j = elem.faces[f];
                dF += fields.F(j, v);
            }
            /// Compute update with source term
            fields.b(i, v) = std::isnan(fields.S(i, v) - dF) ? 0.0 : fields.S(i, v) - dF;

            /// Advance solution with stage coefficient
            fields.W(i, v) = fields.Wold(i, v) + a[inner_iter] * dt / elem.volume * fields.b(i, v);
        }
    }
    /// Update stage counter
    inner_iter = (inner_iter + 1) % input.numerical.time_stages;
}

} // namespace eulercpp::math
