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
 * @file load_bc.hpp
 * @brief Boundary condition loading interface.
 *
 * This header defines the boundary condition types and data structures
 * used in the solver. It also declares the function responsible for
 * parsing and populating boundary conditions from a configuration map.
 *
 * Boundary conditions specify how the computational domain interacts
 * with its surroundings. They are identified by integer codes in the
 * input configuration and mapped to the BCType enum. Each boundary can
 * be spatially localized (via xmin, xmax, etc.) and may carry up to
 * five real parameters (`value[]`) whose meaning depends on the type
 * of boundary condition.
 *
 * @author Alessio Improta
 */

#pragma once

#include <array>
#include <map>
#include <string>
#include <vector>
#include <limits>

namespace eulercpp::physics {

/**
 * @enum BCType
 * @brief Boundary condition types supported by the solver.
 *
 * Enumeration values map to integer codes as read from the input file:
 *
 * | Code | Type               | Description                |
 * |------|--------------------|----------------------------|
 * | 0    | SUPERSONIC_INLET   | Supersonic inflow          |
 * | 1    | SUPERSONIC_OUTLET  | Supersonic outflow         |
 * | 2    | STAGNATION_INLET   | Stagnation inlet           |
 * | 3    | SUBSONIC_INLET     | Subsonic inflow            |
 * | 4    | PRESSURE_OUTLET    | Pressure outlet            |
 * | 5    | WALL               | No-slip wall               |
 * | 6    | SYMMETRY           | Symmetry condition         |
 * | 7    | SLIPWALL           | Slip wall                  |
 * | 8    | MOVING_WALL        | Moving wall                |
 * | 9    | AXIS               | Axis condition             |
 *
 * ### Meaning of `boundary.value[5]` depending on type:
 *
 * - **SUPERSONIC_INLET (0):**
 *   - `value[0] = Mach`
 *   - `value[1] = Pressure`
 *   - `value[2] = Temperature`
 *   - `value[3] = α (alpha, flow direction angle)`
 *   - `value[4] = φ (phi, flow direction angle)`
 *
 * - **SUPERSONIC_OUTLET (1), WALL (5), SYMMETRY (6),
 *   SLIPWALL (7), MOVING_WALL (8), AXIS (9):**
 *   - All `value[]` entries ignored.
 *   - Note: WALL, SYMMETRY, SLIPWALL, MOVING_WALL are treated
 *     identically because the solver is inviscid.
 *
 * - **PRESSURE_OUTLET (4):**
 *   - `value[0] = static pressure`
 *   - Remaining entries ignored.
 *
 * - **STAGNATION_INLET (2):**
 *   - `value[0] = H_tot (total enthalpy)`
 *   - `value[1] = P_tot (total pressure)`
 *   - `value[2] = P_supersonic`
 *   - `value[3] = α (alpha, flow direction angle)`
 *   - `value[4] = φ (phi, flow direction angle)`
 *
 * - **SUBSONIC_INLET (3):**
 *   - `value[0] = Temperature`
 *   - `value[1] = u (x-velocity)`
 *   - `value[2] = v (y-velocity)`
 *   - `value[3] = w (z-velocity)`
 *   - `value[4]` unused.
 */
enum class BCType {
    SUPERSONIC_INLET,   /**< 0 */
    SUPERSONIC_OUTLET,  /**< 1 */
    STAGNATION_INLET,   /**< 2 */
    SUBSONIC_INLET,     /**< 3 */
    PRESSURE_OUTLET,    /**< 4 */
    WALL,               /**< 5 */
    SYMMETRY,           /**< 6 */
    SLIPWALL,           /**< 7 */
    MOVING_WALL,        /**< 8 */
    AXIS,               /**< 9 */
};

} // namespace eulercpp::physics

namespace eulercpp {

struct Input;

/**
 * @struct Boundary
 * @brief Describes a single boundary of the computational domain.
 *
 * Each boundary has:
 * - an identifier (`id`)
 * - a type (`BCType`)
 * - spatial limits (xmin–zmax)
 * - up to 5 input parameters (`value[]`)
 * - a computed physical state (`state[]`)
 */
struct Boundary {
    int id = 0;  /**< Boundary identifier. */
    physics::BCType type = physics::BCType::SYMMETRY; /**< Boundary type. */

    double xmin = -std::numeric_limits<double>::max(); /**< Min X */
    double xmax =  std::numeric_limits<double>::max(); /**< Max X */
    double ymin = -std::numeric_limits<double>::max(); /**< Min Y */
    double ymax =  std::numeric_limits<double>::max(); /**< Max Y */
    double zmin = -std::numeric_limits<double>::max(); /**< Min Z */
    double zmax =  std::numeric_limits<double>::max(); /**< Max Z */

    std::array<double, 3> center = {0.0}; /**< Center */
    double radius = std::numeric_limits<double>::max(); /**< Radius */

    std::array<double, 5> value; /**< Input values. */
    std::array<double, 5> state; /**< Computed state. */
};

/**
 * @struct BoundaryConditions
 * @brief Container for all boundary conditions.
 */
struct BoundaryConditions {
    int n_boundaries = 0;             /**< Number of boundaries. */
    std::vector<Boundary> boundaries; /**< List of boundaries. */
};

/**
 * @brief Populate boundary conditions from a configuration map.
 *
 * This function reads boundary configuration parameters from
 * a map of string key–value pairs and updates the `Input` structure.
 *
 * @param config Map of configuration key–value pairs.
 * @param input  Input structure to update.
 */
void load_bc(const std::map<std::string, std::string>& config, Input& input);

} // namespace eulercpp
