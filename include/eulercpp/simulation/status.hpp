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
 * @file status.hpp
 * @brief Defines the Status structure tracking simulation runtime information.
 *
 * This header provides the Status structure used to monitor the
 * progress of a simulation, including iteration counts, time, CFL number 
 * and stop conditions.
 *
 * @author Alessio Improta
 */

#pragma once

namespace eulercpp {

/**
 * @brief Tracks the current runtime status of a simulation.
 */
struct Status {
    /// @brief Current iteration number
    int iteration = 0;

    /// @brief Current time step size
    double dt = 0.0;

    /// @brief Simulation time elapsed
    double time = 0.0;

    /// @brief CFL number used for stability monitoring
    double cfl = 0.0;

    /// @brief Flag indicating whether the simulation has been stopped
    bool stopped = false;
};

} // namespace eulercpp
