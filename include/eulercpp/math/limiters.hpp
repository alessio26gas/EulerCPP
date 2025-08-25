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
 * @file limiters.hpp
 * @brief Limiter functions for numerical reconstruction schemes.
 *
 * This header defines commonly used limiter functions for TVD (Total
 * Variation Diminishing) schemes and other second-order reconstructions.
 * It provides both the enumeration of limiter types and their corresponding
 * inline implementations. A function is provided to map a limiter type to
 * its function pointer.
 *
 * Available limiters:
 * - Minmod
 * - Superbee
 * - Van Leer
 * - Venkatakrishnan
 * - Modified Venkatakrishnan
 *
 * These limiters are used to prevent non-physical oscillations in
 * numerical solutions of hyperbolic PDEs.
 *
 * @author Alessio Improta
 */

#pragma once

#include <cmath>
#include <algorithm>
#include <functional>
#include <stdexcept>

namespace eulercpp::math {

/**
 * @enum Limiter
 * @brief Types of limiters used in numerical schemes.
 *
 * This enum defines the available limiter types.
 */
enum class Limiter {
    MINMOD,             /**< Minmod limiter */
    SUPERBEE,           /**< Superbee limiter */
    VANLEER,            /**< Van Leer limiter */
    VENKATAKRISHNAN,    /**< Venkatakrishnan limiter */
    MODVENKATAKRISHNAN, /**< Modified Venkatakrishnan limiter */
};

/**
 * @brief Minmod limiter.
 *
 * The Minmod limiter tends to be highly diffusive, making it a good 
 * choice for testing purposes or verifying new code modules.
 *
 * This limiter provides a monotonic reconstruction by returning 1 if 
 * the ratio is less than 1, and 1/rf otherwise.
 *
 * @param rf The ratio of differences used in the reconstruction.
 * @return The limited value.
 */
inline double minmod(double rf) {
    return (rf < 1.0) ? 1.0 : 1.0 / rf;
}

/**
 * @brief Superbee limiter.
 *
 * The Superbee limiter is more aggressive in preserving sharp gradients 
 * than most other second-order limiters.
 *
 * It returns 2.0 for rf < 0.5, and computes a value using rf for rf >= 0.5.
 *
 * @param rf The ratio of differences used in the reconstruction.
 * @return The limited value.
 */
inline double superbee(double rf) {
    if (rf < 0.5) return 2.0;
    return std::max(std::min(2.0 / rf, 1.0), std::min(1.0 / rf, 2.0));
}

/**
 * @brief Van Leer limiter.
 *
 * The Van Leer limiter is a classic, symmetric second-order TVD limiter. 
 * It is known for being robust while introducing less numerical diffusion 
 * than the Minmod limiter.
 *
 * It returns 2.0 / (rf + 1.0).
 *
 * @param rf The ratio of differences used in the reconstruction.
 * @return The limited value.
 */
inline double vanleer(double rf) {
    return 2.0 / (rf + 1.0);
}

/**
 * @brief Venkatakrishnan limiter.
 *
 * Venkatakrishnan's limiter is widely used because of its superior 
 * convergence properties. This limiter often depends on a parameter 
 * intended to control the amount of limiting. Setting it to zero results 
 * in full limiting, but this may stall the convergence. Contrary to that, 
 * if the parameter is set to a large value, the limiter function will 
 * return a value of about unity. Hence, there will be no limiting at all 
 * and wiggles could occur in the solution.
 * In this implementation the parameter is set to zero (K = 0).
 *
 * @param rf The ratio of differences used in the reconstruction.
 * @return The limited value.
 */
inline double venkatakrishnan(double rf) {
    return (2.0 * rf + 1.0) / (rf * (2.0 * rf + 1.0) + 1.0);
}

/**
 * @brief Modified Venkatakrishnan limiter.
 *
 * Modified version of the Venkatakrishnan limiter.
 *
 * @param rf The ratio of differences used in the reconstruction.
 * @return The limited value.
 */
inline double modified_venkatakrishnan(double rf) {
    return (rf * (2.0 * rf + 1.0) + 1.0) /
           (rf * (rf * (2.0 * rf + 1.0) + 1.0) + 1.0);
}

/**
 * @brief Type alias for a limiter function.
 *
 * This type alias defines a function pointer type for limiter functions 
 * that take a double as input and return a double.
 */
using LimiterFunction = double(*)(double);

/**
 * @brief Returns the appropriate limiter function based on the specified type.
 *
 * This function maps the Limiter enum to its corresponding limiting function.
 *
 * @param type The limiter type.
 * @return The corresponding limiter function.
 * @throws std::invalid_argument if the type is unknown.
 */
inline LimiterFunction get_limiter(Limiter type) {
    switch (type) {
        case Limiter::MINMOD:             return &minmod;
        case Limiter::SUPERBEE:           return &superbee;
        case Limiter::VANLEER:            return &vanleer;
        case Limiter::VENKATAKRISHNAN:    return &venkatakrishnan;
        case Limiter::MODVENKATAKRISHNAN: return &modified_venkatakrishnan;
        default: throw std::invalid_argument("Unknown limiter.");
    }
}

} // namespace eulercpp::math
