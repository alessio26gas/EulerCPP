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
 * @file pressure_outlet.hpp
 * @brief Pressure outlet boundary condition.
 *
 * Declares the function interface for applying a pressure outlet boundary.
 *
 * @author Alessio Improta
 */

#pragma once

namespace eulercpp {
    struct Input;
    struct Face;
    struct Fields;
    struct Boundary;
} // namespace eulercpp

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
);

} // namespace eulercpp::physics::bc
