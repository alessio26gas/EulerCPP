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
 * @file supersonic_inlet.hpp
 * @brief Supersonic inlet boundary condition.
 *
 * Declares the interface for initializing and applying a supersonic inlet
 * boundary condition.
 *
 * At supersonic inlets, all flow variables are prescribed, meaning no
 * information propagates upstream. The state is constructed from Mach
 * number, static pressure, temperature, and flow direction angles.
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
void init_supersonic_inlet(Boundary& bc, const Simulation& sim);

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
);

} // namespace eulercpp::physics::bc
