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
 * @file timestep.cpp
 * @brief Implements timestep computation.
 *
 * This file provides the implementation of functions that compute
 * the next simulation timestep based on CFL constraints, fluid velocity,
 * speed of sound, and element geometry.
 *
 * @author Alessio Improta
 */

#include <array>
#include <omp.h>

#include <eulercpp/simulation/simulation.hpp>

namespace eulercpp::physics {

/**
 * @brief Compute and update the simulation timestep.
 *
 * This function computes the maximum allowable timestep for the simulation
 * based on CFL constraints. The timestep is calculated using the local
 * velocities, speed of sound, and element geometry.
 *
 * The computation is parallelized over mesh elements using OpenMP.
 *
 * @param sim Reference to the Simulation object.
 */
void update_timestep(Simulation& sim) {
    const auto& mesh = sim.mesh;
    const auto& input = sim.input;
    auto& status = sim.status;
    auto& fields = sim.fields;

    double var = 0.0;

    #pragma omp parallel
    {
        double var_local = 0.0;

        #pragma omp for nowait
        for (int i = 0; i < mesh.n_elements; ++i) {
            const auto& elem = mesh.elements[i];
            const double rho = fields.W(i, 0);
            const double u = fields.W(i, 1) / rho;
            const double v = fields.W(i, 2) / rho;
            const double w = fields.W(i, 3) / rho;
            const double K = 0.5*rho*(u*u+v*v+w*w);
            const double p = (input.fluid.gamma-1.0)*(fields.W(i, 4) - K);
            const double a = std::sqrt(input.fluid.gamma * p / rho);

            double l_max = 0.0;
            for (int f = 0; f < elem.n_faces; ++f) {
                const int fi = elem.faces[f];
                const auto& face = mesh.faces[fi];
                std::array<double, 3> n = face.normal;
                const double un = u*n[0] + v*n[1] + w*n[2];
                l_max = std::max(l_max, face.area * (un + a));
            }

            double ratio = l_max / elem.volume;
            if (ratio > var_local) {
                var_local = ratio;
            }
        }

        #pragma omp critical
        {
            if (var_local > var) {
                var = var_local;
            }
        }
    }

    double dtconv = status.cfl / var;
    status.dt = dtconv;
    status.time += dtconv;

    if (status.time > input.numerical.maxtime) {
        status.dt -= status.time - input.numerical.maxtime;
        status.time = input.numerical.maxtime;
    }
}

} // namespace eulercpp::physics
