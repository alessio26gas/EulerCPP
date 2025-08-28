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
 * @file initialization.hpp
 * @brief Declaration of functions to initialize simulation fields and setup.
 *
 * This file declares functions responsible for setting the initial conditions
 * of the simulation, initializing limiters and reconstruction schemes,
 * configuring axisymmetry if needed, and setting up output formats.
 *
 * Functions also handle restart file loading for continuing simulations.
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/simulation/simulation.hpp>

namespace eulercpp {

/**
 * @brief Set the initial conditions of the simulation.
 *
 * This function initializes the conservative variables in the simulation
 * either from a restart file or from default input values. It supports:
 * - Parallel initialization using OpenMP
 * - Block-specific initial conditions
 * - Error handling for mismatched restart files
 *
 * @param sim Reference to the Simulation object to initialize.
 * @throws std::runtime_error if the restart file cannot be read or
 * contains errors.
 */
void set_initial_conditions(Simulation& sim);

/**
 * @brief Initialize simulation components and numerical schemes.
 *
 * This function performs the following:
 * - Initializes the numerical limiter
 * - Selects the reconstruction scheme
 * - Configures axisymmetric physics if needed
 * - Sets up output format and output folder for results
 *
 * @param sim Reference to the Simulation object to initialize.
 */
void initialize_simulation(Simulation& sim);

} // namespace eulercpp
