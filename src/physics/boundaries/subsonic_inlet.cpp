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
 * @file subsonic_inlet.cpp
 * @brief Subsonic inlet boundary condition implementation.
 *
 * Defines the function that applies a subsonic inlet boundary condition.
 *
 * @author Alessio Improta
 */

#include <eulercpp/input/input.hpp>
#include <eulercpp/mesh/faces.hpp>
#include <eulercpp/simulation/fields.hpp>
#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/input/load_bc.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp::physics::bc {

/**
 * @brief Apply a subsonic inlet boundary condition.
 *
 * @param input   Global simulation input including fluid properties.
 * @param face    Face geometry and orientation vectors.
 * @param f       Index of the face in the mesh.
 * @param bc      Boundary condition data.
 * @param fields  Simulation fields.
 */
void subsonic_inlet(
    const Input& input,
    const Face& face,
    const int f,
    const Boundary& bc,
    Fields& fields
) {
    const double T = bc.value[0];
    const double u = bc.value[1];
    const double v = bc.value[2];
    const double w = bc.value[3];
    const double k = 0.5 * (u*u+v*v+w*w);

    const auto& n = face.normal;
    const double R = input.fluid.R;
    const double gam = input.fluid.gamma;

    const double rhoext = fields.Wf(f, 0);
    const double Eext = fields.Wf(f, 4);
    const double uext = fields.Wf(f, 1)/rhoext;
    const double vext = fields.Wf(f, 2)/rhoext;
    const double wext = fields.Wf(f, 3)/rhoext;
    const double kext = 0.5 * (uext*uext+vext*vext+wext*wext);
    const double p = (gam-1.0)*(Eext-rhoext*kext);

    const double rho = p / R / T;
    const double un = u * n[0] + v * n[1] + w * n[2];
    const double E = p / (gam - 1.0) + rho * k;

    fields.F(f, 0) = rho * un;
    fields.F(f, 1) = p * n[0] + rho * u * un;
    fields.F(f, 2) = p * n[1] + rho * v * un;
    fields.F(f, 3) = p * n[2] + rho * w * un;
    fields.F(f, 4) = (E + p) * un;
}

} // namespace eulercpp::physics::bc
