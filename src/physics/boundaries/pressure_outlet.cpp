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
 * @file pressure_outlet.cpp
 * @brief Pressure outlet boundary condition implementation.
 *
 * Defines the function that applies a pressure outlet boundary condition.
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
 * @brief Apply a pressure outlet boundary condition.
 *
 * At a pressure outlet, the static pressure is fixed to a prescribed value
 * from the boundary condition specification. If the flow is subsonic normal
 * to the outlet, the pressure is imposed using characteristic relations. For
 * supersonic outflow, the solution is extrapolated directly from the interior.
 *
 * @param input   Global simulation input including fluid properties.
 * @param face    Face geometry and orientation vectors.
 * @param f       Index of the face in the mesh.
 * @param bc      Boundary condition data.
 * @param fields  Simulation fields (state variables and fluxes).
 */
void pressure_outlet(
    const Input& input,
    const Face& face,
    const int f,
    const Boundary& bc,
    Fields& fields
) {
    const auto& n = face.normal;
    double rho = fields.Wf(f, 0);
    double E = fields.Wf(f, 4);
    double u = fields.Wf(f, 1)/rho;
    double v = fields.Wf(f, 2)/rho;
    double w = fields.Wf(f, 3)/rho;
    double p = (input.fluid.gamma-1.0)*(E-0.5*rho*(u*u+v*v+w*w));
    if (p < 0) p = 1.0e-14;
    double un = u * n[0] + v * n[1] + w * n[2];

    double a = std::sqrt(input.fluid.gamma * p / rho);
    if (un < a) {
        const double pb = bc.value[0];

        const auto& t1 = face.t1, t2 = face.t2;
        const double ut1 = u * t1[0] + v * t1[1] + w * t1[2];
        const double ut2 = u * t2[0] + v * t2[1] + w * t2[2];
        const double gamma  = a * a * rho / p;

        const double expo  = (gamma - 1.0) / (2.0 * gamma);
        const double ab   = a * std::pow(pb / p, expo);
        const double unb  = un + 2.0 / (gamma - 1.0) * (a - ab);
        const double rhob = gamma * pb / (ab * ab);

        const double ubx = n[0] * unb + t1[0] * ut1 + t2[0] * ut2;
        const double uby = n[1] * unb + t1[1] * ut1 + t2[1] * ut2;
        const double ubz = n[2] * unb + t1[2] * ut1 + t2[2] * ut2;

        const double V2  = ubx * ubx + uby * uby + ubz * ubz;
        const double Eb  = pb / (gamma - 1.0) + 0.5 * rhob * V2;

        rho = rhob;
        p   = pb;
        u   = ubx; v = uby; w = ubz;
        E   = Eb;
        un  = unb;
    }

    fields.F(f, 0) = rho * un;
    fields.F(f, 1) = p * n[0] + rho * u * un;
    fields.F(f, 2) = p * n[1] + rho * v * un;
    fields.F(f, 3) = p * n[2] + rho * w * un;
    fields.F(f, 4) = (E + p) * un;
}

} // namespace eulercpp::physics::bc
