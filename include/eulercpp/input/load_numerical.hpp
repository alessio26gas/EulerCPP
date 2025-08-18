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
    @file load_numerical.hpp
    @brief Input handling for numerical solver settings.

    Declares the structure and function to load numerical parameters
    from a key-value configuration map. Includes reconstruction scheme,
    limiter, time integration settings, CFL, maximum time, and iteration limits.

    @author Alessio Improta
*/

#pragma once

#include <map>
#include <string>
#include <vector>

#include <eulercpp/math/limiters.hpp>
#include <eulercpp/math/reconstruction.hpp>
#include <eulercpp/physics/riemann.hpp>

namespace eulercpp {

struct Input;

/**
 * @struct Numerical
 * @brief Holds all input numerical settings.
 */
struct Numerical {
    /** Reconstruction scheme. */
    math::Reconstruction reconstruction = math::Reconstruction::CONSTANT;

    /** Limiter function. */
    math::Limiter limiter = math::Limiter::MINMOD;

    /** Riemann solver. */
    physics::Riemann riemann = physics::Riemann::HLLC;

    int time_stages = 1;    /**< Number of stages for multi-stage time integration scheme. */
    std::vector<double> a;  /**< Multi-stage coefficients. */
    double CFL = 0.8;       /**< CFL condition number for time stepping. */
    double maxtime = 1.0;   /**< Maximum simulation time. */
    int maxiter = 1e3;      /**< Maximum number of iterations. */
};

/**
 * @brief Populate numerical parameters from a configuration map.
 *
 * Reads time integration settings, CFL number, maximum time/iterations,
 * reconstruction method, and limiter choice. Ensures consistency between
 * time stages and coefficients.
 *
 * @param config Map of configuration key-value pairs.
 * @param input  Input structure to update with numerical parameters.
 *
 * @throws std::invalid_argument if the number of coefficients does 
 *         not match the number of time stages or if the number of 
 *         stages is invalid.
 */
void load_numerical(const std::map<std::string, std::string>& config, Input& input);

} // namespace eulercpp
