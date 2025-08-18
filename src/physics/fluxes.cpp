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
 * @file fluxes.cpp
 * @brief Implements convective flux computation.
 *
 * This file contains the implementation of convective flux computation
 * across mesh faces. It uses a Riemann solver in the face-normal coordinate 
 * system and projects the fluxes back to the global coordinate system.
 *
 * Parallelized with OpenMP for improved performance.
 *
 * @author Alessio Improta
 */

#include <algorithm>
#include <cmath>
#include <omp.h>

#include <eulercpp/math/vectors.hpp>
#include <eulercpp/physics/riemann.hpp>
#include <eulercpp/simulation/simulation.hpp>

namespace eulercpp::physics {

/**
 * @brief Compute convective fluxes across all mesh faces.
 *
 * For each face, the function:
 * 1. Extracts the left and right states in the face-normal coordinate system.
 * 2. Calls the Riemann solver to compute fluxes across the face.
 * 3. Maps the fluxes back to the global coordinate system.
 * 4. Scales the fluxes by the face area.
 *
 * This operation is parallelized using OpenMP.
 *
 * @param sim Reference to the Simulation object.
 */
void compute_fluxes(Simulation& sim) {
    const auto& mesh = sim.mesh;
    auto& fields = sim.fields;
    const int n_var = 5;

    #pragma omp parallel for
    for (int i = 0; i < mesh.n_faces; ++i) {
        const auto& face = mesh.faces[i];
        const int j = face.opposite;
        if (j < 0) continue;

        const auto& n  = face.normal;
        const auto& t1 = face.t1;
        const auto& t2 = face.t2;

        std::array<double, 5> WL, WR, Fr;

        WL[0] = fields.Wf(i, 0);
        WL[1] = fields.Wf(i, 1) * n[0] +
                fields.Wf(i, 2) * n[1] +
                fields.Wf(i, 3) * n[2];
        WL[2] = fields.Wf(i, 1) * t1[0] +
                fields.Wf(i, 2) * t1[1] +
                fields.Wf(i, 3) * t1[2];
        WL[3] = fields.Wf(i, 1) * t2[0] +
                fields.Wf(i, 2) * t2[1] +
                fields.Wf(i, 3) * t2[2];
        WL[4] = fields.Wf(i, 4);

        WR[0] = fields.Wf(j, 0);
        WR[1] = fields.Wf(j, 1) * n[0] +
                fields.Wf(j, 2) * n[1] +
                fields.Wf(j, 3) * n[2];
        WR[2] = fields.Wf(j, 1) * t1[0] +
                fields.Wf(j, 2) * t1[1] +
                fields.Wf(j, 3) * t1[2];
        WR[3] = fields.Wf(j, 1) * t2[0] +
                fields.Wf(j, 2) * t2[1] +
                fields.Wf(j, 3) * t2[2];
        WR[4] = fields.Wf(j, 4);

        riemann(WL.data(), WR.data(), Fr.data(), sim.input.fluid.gamma);

        fields.F(i, 0) = Fr[0];
        fields.F(i, 1) = Fr[1] * n[0] + Fr[2] * t1[0] + Fr[3] * t2[0];
        fields.F(i, 2) = Fr[1] * n[1] + Fr[2] * t1[1] + Fr[3] * t2[1];
        fields.F(i, 3) = Fr[1] * n[2] + Fr[2] * t1[2] + Fr[3] * t2[2];
        fields.F(i, 4) = Fr[4];

        const double A = face.area;
        for (int v = 0; v < n_var; ++v) {
            fields.F(i, v) *= A;
        }
    }
}

} // namespace eulercpp::physics
