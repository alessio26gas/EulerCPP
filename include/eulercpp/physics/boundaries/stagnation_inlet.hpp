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
 * @file stagnation_inlet.hpp
 * @brief Stagnation inlet boundary condition.
 *
 * Declares functions for initializing and applying a stagnation inlet
 * boundary condition.
 *
 * @author Alessio Improta
 */

#pragma once

namespace eulercpp {
    struct Simulation;
    struct Input;
    struct Face;
    struct Fields;
    struct Boundary;
} // namespace eulercpp

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
void init_stagnation_inlet(Boundary& bc, const Simulation& sim);

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
);

} // namespace eulercpp::physics::bc
