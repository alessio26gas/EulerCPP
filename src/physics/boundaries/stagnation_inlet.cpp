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
 * @file stagnation_inlet.cpp
 * @brief Stagnation inlet boundary condition implementation.
 *
 * Implements a stagnation inlet boundary condition for Euler CFD.
 *
 * - **Initialization**:
 *   Converts stagnation enthalpy and pressure into a conservative
 *   state assuming a supersonic inflow.
 *
 * - **Application**:
 *   Depending on the face-normal velocity:
 *   - Supersonic inflow: impose precomputed state.
 *   - Subsonic inflow:   derive static pressure and velocity
 *                        from total conditions and orientation.
 *   - Reverse flow:      handled as pressure outlet (subsonic)
 *                        or supersonic outlet.
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
 * @brief Initialize a stagnation inlet state.
 *
 * @param bc   Boundary condition object containing values:
 *             - `value[0]`: total enthalpy,
 *             - `value[1]`: total pressure,
 *             - `value[2]`: supersonic static pressure,
 *             - `value[3]`: flow angle alpha,
 *             - `value[4]`: flow angle phi.
 * @param sim  Simulation object.
 */
void init_stagnation_inlet(Boundary& bc, const Simulation& sim) {
    const double PI = 3.14159265358979323846;
    bc.value[3] *= PI / 180.0;
    bc.value[4] *= PI / 180.0;

    const double Htot = bc.value[0];
    const double Ptot = bc.value[1];
    const double Psup = bc.value[2];
    const double alpha = bc.value[3];
    const double phi = bc.value[4];

    const double R = sim.input.fluid.R;
    const double gam = sim.input.fluid.gamma;
    const double gam1 = gam-1.0;
    const double gam2 = 2.0/gam1;
    const double gam3 = gam/gam1;

    const double M = sqrt(gam2 * (pow(Ptot / Psup, 1.0/gam3) - 1.0));
    const double T = Htot / (R * gam3) / (1.0 + 0.5*gam1*M*M);

    const double rho = Psup / T / R;
    const double V = M * std::sqrt(gam * Psup / rho);
    const double u = V * std::cos(alpha) * std::cos(phi);
    const double v = V * std::sin(alpha) * std::cos(phi);
    const double w = V * std::sin(phi);
    const double E = Psup / gam1 + 0.5 * rho * V * V;

    bc.state = {rho, u, v, w, E};
}

/**
 * @brief Apply a stagnation inlet boundary condition.
 *
 * Handles multiple possible flow regimes:
 * - **Supersonic inlet**: all variables are imposed.
 * - **Subsonic inlet**: static pressure is determined from
 *   total conditions and flow direction.
 * - **Reverse flow**: treated as a pressure outlet if subsonic,
 *   or a supersonic outlet otherwise.
 *
 * @param input   Simulation input.
 * @param face    Mesh face geometry and normal/tangent vectors.
 * @param f       Index of the face in the mesh.
 * @param bc      Boundary condition data.
 * @param fields  Simulation fields.
 */
void stagnation_inlet(
    const Input& input,
    const Face& face,
    const int f,
    const Boundary& bc,
    Fields& fields
) {
    const double Htot = bc.value[0];
    const double Ptot = bc.value[1];
    const double Psup = bc.value[2];
    const double alpha = bc.value[3];
    const double phi = bc.value[4];

    const auto& n = face.normal;
    const auto& t1 = face.t1, t2 = face.t2;

    const double R = input.fluid.R;
    const double gam = input.fluid.gamma;
    const double gam1 = gam-1.0;
    const double gam2 = 2.0/gam1;
    const double gam3 = gam/gam1;

    double rho = fields.Wf(f, 0);
    double u = fields.Wf(f, 1)/rho;
    double v = fields.Wf(f, 2)/rho;
    double w = fields.Wf(f, 3)/rho;
    double un = u * n[0] + v * n[1] + w * n[2];
    double E = fields.Wf(f, 4);
    double p = gam1*(E-0.5*rho*(u*u+v*v+w*w));
    if (p < 0.0) p = 1.0e-14;

    double a = std::sqrt(gam * p / rho);
    if (un < - a) {
        // Supersonic inlet condition
        p = Psup;
        rho = bc.state[0];
        u = bc.state[1];
        v = bc.state[2];
        w = bc.state[3];
        un = u * n[0] + v * n[1] + w * n[2];
        E = bc.state[4];

    } else if (un < 0.0) {
        // Subsonic inlet condition
        const double sigma = std::cos(alpha)*std::cos(phi) * n[0] +
                             std::sin(alpha)*std::cos(phi) * n[1] +
                             std::sin(phi) * n[2];

        const double Rp = un + a * gam2;
        const double A = sigma*sigma + gam2;
        const double B = - 2.0 * sigma * Rp;
        const double C = Rp*Rp - Htot * 2.0 * gam2;
        const double V = (- B + sqrt(B*B - 4.0*A*C)) / (2.0*A);

        const double k = 0.5*V*V;
        const double T = (Htot - k) / (R*gam3);

        p = Ptot / pow(1.0 + k/(gam3*R*T), gam3);
        rho = p / (R * T);
        u = V * cos(alpha) * cos(phi);
        v = V * sin(alpha) * cos(phi);
        w = V * sin(phi);
        un = u * n[0] + v * n[1] + w * n[2];
        E = p/gam1 + rho*k;

    } else {
        // Reverse flow
        if (un < a) {
            // Pressure outlet condition
            const double ut1 = u * t1[0] + v * t1[1] + w * t1[2];
            const double ut2 = u * t2[0] + v * t2[1] + w * t2[2];

            const double ab = a * std::pow(Ptot / p, 0.5/gam3);

            p   = Ptot;
            rho = gam * p / (ab * ab);
            un  = un + gam2 * (a - ab);
            u   = n[0] * un + t1[0] * ut1 + t2[0] * ut2;
            v   = n[1] * un + t1[1] * ut1 + t2[1] * ut2;
            w   = n[2] * un + t1[2] * ut1 + t2[2] * ut2;
            E   = p/gam1 + 0.5*rho*(u*u+v*v+w*w);

        } else {
            // Supersonic outlet condition
        }
    }

    fields.F(f, 0) = rho * un;
    fields.F(f, 1) = p * n[0] + rho * u * un;
    fields.F(f, 2) = p * n[1] + rho * v * un;
    fields.F(f, 3) = p * n[2] + rho * w * un;
    fields.F(f, 4) = (E + p) * un;
}

} // namespace eulercpp::physics::bc
