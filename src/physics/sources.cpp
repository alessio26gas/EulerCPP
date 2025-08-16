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
 * @file sources.cpp
 * @brief Implements source term computation.
 *
 * This file provides the implementation of source term updates. Source 
 * terms are scaled by element volume and computed in parallel with OpenMP.
 *
 * @author Alessio Improta
 */

#include <omp.h>

#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/physics/axisymmetric.hpp>

namespace eulercpp::physics {

/**
 * @brief Compute and update source terms for the simulation.
 *
 * This function resets all source terms to zero, applies any physics-specific
 * source contributions and scales the sources by the volume of each element.
 *
 * @param sim Reference to the Simulation object.
 */
void update_sources(Simulation& sim) {
    const auto& input = sim.input;
    const auto& mesh = sim.mesh;
    auto& fields = sim.fields;

    #pragma omp parallel for
    for (int i = 0; i < mesh.n_elements; ++i) {
        for (int v = 0; v < 5; ++v) {
            fields.S(i, v) = 0.0;
        }
    }

    if (input.physics.dimension == 2) {
        axisymmetry_sources(sim);
    }

    #pragma omp parallel for
    for (int i = 0; i < mesh.n_elements; ++i) {
        const double V = mesh.elements[i].volume;
        for (int v = 0; v < 5; ++v) {
            fields.S(i, v) *= V;
        }
    }
}

} // namespace eulercpp::physics
