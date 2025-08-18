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
 * @file timestep.hpp
 * @brief Declaration of timestep update routines.
 *
 * This file declares functions that compute the next timestep based on
 * CFL constraints using the current state of the simulation.
 *
 * Parallel computation using OpenMP is supported for efficiency.
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/simulation/simulation.hpp>

namespace eulercpp::physics {

/**
 * @brief Compute and update the simulation timestep.
 *
 * This function computes the maximum allowable timestep for the simulation
 * based on CFL constraints. The timestep is calculated using the local
 * velocities, speed of sound, and element geometry.
 *
 * The computation is parallelized over mesh elements using OpenMP.
 *
 * @param sim Reference to the Simulation object.
 */
void update_timestep(Simulation& sim);

} // namespace eulercpp::physics
