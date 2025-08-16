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
 * @file symmetry.cpp
 * @brief Symmetry boundary condition implementation.
 *
 * Defines the function that applies a symmetry boundary condition.
 *
 * @author Alessio Improta
 */

#include <array> 

#include <eulercpp/input/input.hpp>
#include <eulercpp/mesh/faces.hpp>
#include <eulercpp/simulation/fields.hpp>
#include <eulercpp/input/load_bc.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp::physics::bc {

/**
 * @brief Apply a symmetry boundary condition.
 *
 * @param input   Input settings including fluid properties.
 * @param face    Face geometry and orientation data.
 * @param f       Index of the face in the mesh.
 * @param fields  Simulation fields.
 */
void symmetry(
    const Input& input, const Face& face, const int f, Fields& fields
) {
    const auto& n = face.normal;
    const double rho = fields.Wf(f, 0);
    const double E = fields.Wf(f, 4);
    const double u = fields.Wf(f, 1)/rho;
    const double v = fields.Wf(f, 2)/rho;
    const double w = fields.Wf(f, 3)/rho;
    double p = (input.fluid.gamma-1.0)*(E-0.5*rho*(u*u+v*v+w*w));
    if (p < 0) p = 1.0e-14;
    fields.F(f, 1) = p * n[0];
    fields.F(f, 2) = p * n[1];
    fields.F(f, 3) = p * n[2];
}

} // namespace eulercpp::physics::bc
