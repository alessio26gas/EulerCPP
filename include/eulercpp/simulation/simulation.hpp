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
 * @file simulation.hpp
 * @brief Defines the Simulation structure.
 *
 * This header provides the main Simulation structure which aggregates
 * the input parameters, mesh, solution fields, and status of the simulation.
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/input/input.hpp>
#include <eulercpp/mesh/mesh.hpp>
#include <eulercpp/simulation/fields.hpp>
#include <eulercpp/simulation/status.hpp>
#include <eulercpp/math/limiters.hpp>

namespace eulercpp {

/**
 * @brief Represents a complete simulation object.
 *
 * This structure aggregates all major components of a simulation:
 * - Input parameters for configuration
 * - Computational mesh
 * - Fields storing solution variables
 * - Status tracking current iteration, time, and other runtime info
 */
struct Simulation {
    /// @brief User input configuration for the simulation
    Input input;

    /// @brief  Computational mesh defining nodes and elements
    Mesh mesh;

    /// @brief  Fields storing solution variables
    Fields fields;

    /// @brief  Status object tracking runtime progress and control flags
    Status status;
};

} // namespace eulercpp
