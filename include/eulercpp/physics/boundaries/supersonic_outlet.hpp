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
 * @file supersonic_outlet.hpp
 * @brief Supersonic outlet boundary condition implementation.
 *
 * Declares the function interface for applying a supersonic outlet
 * boundary condition. Supersonic outlets allow all variables to convect 
 * freely out of the domain without feedback, enforcing fluxes based on 
 * the interior state.
 *
 * @author Alessio Improta
 */

#pragma once

namespace eulercpp {
    struct Input;
    struct Face;
    struct Fields;
} // namespace eulercpp

namespace eulercpp::physics::bc {

/**
 * @brief Apply a supersonic outlet boundary condition.
 *
 * At a supersonic outlet, all flow variables are extrapolated directly
 * from the interior solution. The fluxes are computed using the conservative
 * state at the boundary face.
 *
 * @param input   Input settings including fluid properties.
 * @param face    Face geometry and normal vector.
 * @param f       Index of the face in the mesh.
 * @param fields  Simulation fields.
 */
void supersonic_outlet(
    const Input& input, const Face& face, const int f, Fields& fields
);

} // namespace eulercpp::physics::bc
