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
 * @file supersonic_inlet.cpp
 * @brief Supersonic inlet boundary condition implementation.
 *
 * Defines functions for handling supersonic inlet boundaries.
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
 * @brief Initialize a supersonic inlet state.
 *
 * Converts user-specified boundary condition parameters into a 
 * conservative state vector.
 *
 * @param bc   Boundary condition object to initialize.
 * @param sim  Simulation object.
 *
 * @note This function must be called before `supersonic_inlet()`
 *       to prepare the `bc.state` values.
 */
void init_supersonic_inlet(Boundary& bc, const Simulation& sim) {
    double M = bc.value[0];
    double p = bc.value[1];
    double T = bc.value[2];
    double alpha = bc.value[3], phi = bc.value[4];

    double R = sim.input.fluid.R;
    double gamma = sim.input.fluid.gamma;
    double V = M * std::sqrt(gamma * R * T);

    double rho = p / R / T;
    double u = V * std::cos(alpha) * std::cos(phi);
    double v = V * std::sin(alpha) * std::cos(phi);
    double w = V * std::sin(phi);
    double E = p / (gamma-1.0) + 0.5*rho*V*V;

    bc.state = {rho, u, v, w, E};
}

/**
 * @brief Apply a supersonic inlet boundary condition.
 *
 * At a supersonic inlet, all conservative flow variables are
 * directly imposed from the initialized boundary state.
 * Boundary fluxes are then computed consistently with the
 * prescribed inlet state.
 *
 * @param input   Global simulation input including fluid properties.
 * @param face    Face geometry and orientation vectors.
 * @param f       Index of the face in the mesh.
 * @param bc      Boundary condition data.
 * @param fields  Simulation fields.
 */
void supersonic_inlet(
    const Input& input, 
    const Face& face,
    const int f,
    const Boundary& bc,
    Fields& fields
) {
    const double rho = bc.state[0];
    const double u = bc.state[1], v = bc.state[2], w = bc.state[3];
    const double E = bc.state[4];
    const double p = bc.value[1];

    const auto& n = face.normal;
    double un = u * n[0] + v * n[1] + w * n[2];

    fields.F(f, 0) = rho * un;
    fields.F(f, 1) = p * n[0] + rho * u * un;
    fields.F(f, 2) = p * n[1] + rho * v * un;
    fields.F(f, 3) = p * n[2] + rho * w * un;
    fields.F(f, 4) = (E + p) * un;
}

} // namespace eulercpp::physics::bc
