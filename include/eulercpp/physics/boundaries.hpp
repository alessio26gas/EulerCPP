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
 * @file boundaries.hpp
 * @brief Boundary conditions handling.
 *
 * Provides initialization and application of physical boundary conditions.
 *
 * Uses OpenMP for parallel processing over boundary faces.
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/simulation/simulation.hpp>

namespace eulercpp::physics {

/**
 * @brief Apply boundary conditions to the simulation fields.
 *
 * Iterates over all boundary faces in the mesh and applies the correct
 * boundary condition. Each condition modifies the solution fluxes at 
 * the boundary.
 *
 * Parallelized with OpenMP for large meshes.
 *
 * @param sim The simulation object containing mesh, fields, and input data.
 *
 * @throws std::runtime_error if an unknown boundary condition is encountered.
 */
void apply_boundary_conditions(Simulation& sim);

/**
 * @brief Initialize boundary conditions before starting the simulation.
 *
 * @param sim The simulation object containing mesh, fields, and input data.
 *
 * @throws std::runtime_error if an unknown boundary condition is encountered.
 */
void init_boundaries(Simulation& sim);

} // namespace eulercpp::physics
