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
 * @file solution_update.hpp
 * @brief Functions for advancing the solution in time.
 *
 * This header defines the interface for updating the solution
 * of the simulation according to the chosen time integration scheme.
 * It includes a single function to advance the solution for one
 * stage of the multi-stage integration.
 *
 * @author Alessio Improta
 */

#pragma once

namespace eulercpp {
    struct Simulation;
}

namespace eulercpp::math {

/**
 * @brief Advances the simulation solution by one time step/stage.
 *
 * This function updates the conservative variables of the simulation
 * based on the numerical fluxes and source terms. It supports multi-stage
 * time integration methods, using an internal counter to select the
 * appropriate stage coefficient.
 *
 * @param sim Reference to the Simulation object containing
 *            mesh, fields, and numerical parameters.
 */
void advance_solution(Simulation& sim);

} // namespace eulercpp::math
