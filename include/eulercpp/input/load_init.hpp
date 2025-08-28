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
 * @file load_init.hpp
 * @brief Declarations for handling initial conditions.
 *
 * This file defines structures and enums to represent initial
 * conditions for the simulation. It also declares the function to
 * load initial conditions from a configuration map.
 *
 * @author Alessio Improta
 */

#pragma once

#include <array>
#include <limits>
#include <map>
#include <string>
#include <vector>

namespace eulercpp {

struct Input;

/**
 * @enum InitialVariables
 * @brief Specifies the type of initial variable definition.
 *
 * - TEMPERATURE_BASED: Initial conditions specified using temperature.
 * - DENSITY_BASED: Initial conditions specified using density.
 */
enum class InitialVariables {
    TEMPERATURE_BASED,  /**< Temperature-based initial variables */
    DENSITY_BASED,      /**< Density-based initial variables */
};


/**
 * @struct Block
 * @brief Defines an XYZ box for block-specific initial conditions.
 *
 * Each block can override the global initial state within its spatial
 * bounds.
 */
struct Block {
    double xmin = -std::numeric_limits<double>::max(); /**< Minimum x bound */
    double xmax =  std::numeric_limits<double>::max(); /**< Maximum x bound */
    double ymin = -std::numeric_limits<double>::max(); /**< Minimum y bound */
    double ymax =  std::numeric_limits<double>::max(); /**< Maximum y bound */
    double zmin = -std::numeric_limits<double>::max(); /**< Minimum z bound */
    double zmax =  std::numeric_limits<double>::max(); /**< Maximum z bound */

    std::array<double, 3> center = {0.0}; /**< Center */
    double radius = std::numeric_limits<double>::max(); /**< Radius */

    std::array<double, 5> W0; /**< Initial state vector for the block */
};

/**
 * @struct InitialConditions
 * @brief Stores all initial condition settings for the simulation.
 *
 * Supports global initial state, restart files, and block-specific
 * overrides.
 */
struct InitialConditions {
    bool restart = false; /**< Use a restart file as initial condition */
    std::string restart_file; /**< Path to restart file */

    /** Mode for initial state definition */
    InitialVariables initial_variables = InitialVariables::TEMPERATURE_BASED;

    std::array<double, 5> W0; /**< Global initial state vector */

    int n_blocks = 0; /**< Number of block-specific regions */
    std::vector<Block> blocks; /**< List of blocks overriding W0 */
};

/**
 * @brief Load initial condition settings from configuration map.
 *
 * This function configures the simulation's initial state in two modes:
 * - **Restart mode**: Loads from a provided restart file.
 * - **Fresh initialization**: Builds initial conditions based on
 *   pressure/density/temperature and velocity inputs.
 *
 * Supports additional initialization blocks with user-defined
 * physical variables and spatial extents.
 *
 * @param config Key-value map of configuration entries.
 * @param input  Input structure to populate with initialization data.
 *
 * @throw std::invalid_argument If `restart=1` but no file is given, or
 *        if an invalid `initial_variables` mode is specified.
 */
void load_init(const std::map<std::string, std::string>& config, Input& input);

} // namespace eulercpp
