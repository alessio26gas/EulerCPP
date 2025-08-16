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
 * @file reconstruction.hpp
 * @brief Defines reconstruction schemes for numerical flux evaluation.
 *
 * This header provides support for reconstructing solution values at
 * cell faces from cell-centered values. Two main reconstruction schemes
 * are implemented:
 * 
 * - CONSTANT: piecewise constant reconstruction.
 * - MUSCL   : Monotone Upstream-Centered Scheme for Conservation Laws.
 *
 * Users can select a reconstruction scheme and a limiter type for MUSCL.
 * The header declares the public interface for initializing and applying
 * reconstruction.
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/math/limiters.hpp>

namespace eulercpp {
    struct Simulation; /**< Forward declaration of Simulation structure */
}

namespace eulercpp::math {

/**
 * @enum Reconstruction
 * @brief Supported reconstruction schemes.
 */
enum class Reconstruction {
    CONSTANT, /**< Piecewise constant reconstruction */
    MUSCL     /**< MUSCL second-order reconstruction with limiter */
};

/**
 * @brief Function pointer type for reconstruction schemes.
 */
using ReconstructionScheme = void(*)(eulercpp::Simulation&);

/**
 * @brief Applies the current reconstruction scheme to a simulation.
 *
 * Calls the function pointer set by `init_reconstruction`.
 *
 * @param sim Simulation object to apply reconstruction to.
 */
void reconstruction(eulercpp::Simulation& sim);

/**
 * @brief Initializes the reconstruction scheme.
 *
 * Maps the `Reconstruction` enum to the corresponding internal function.
 *
 * @param scheme Selected reconstruction scheme.
 * @throws std::invalid_argument if the scheme is unknown.
 */
void init_reconstruction(Reconstruction scheme);

/**
 * @brief Initializes the limiter function for MUSCL reconstruction.
 *
 * Must be called before using MUSCL reconstruction to ensure the limiter
 * function is set.
 *
 * @param type Limiter type.
 */
void init_limiter(Limiter type);

} // namespace eulercpp::math
