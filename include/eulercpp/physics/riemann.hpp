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
 * @file riemann.hpp
 * @brief Riemann solver declarations.
 *
 * Implements the exact/approximate Riemann solver for convective flux
 * computation across faces.
 *
 * Provides a single interface function `riemann` to compute fluxes
 * from left and right states in the face-normal direction.
 *
 * @author Alessio Improta
 */

#pragma once

namespace eulercpp::physics {

/**
 * @enum Riemann
 * @brief Supported Riemann solvers.
 */
enum class Riemann {
    RUSANOV,    ///< Rusanov solver
    HLL,        ///< HLL (Harten-Lax-van Leer) solver
    HLLC,       ///< HLLC (Harten-Lax-van Leer-Contact) solver
};

/**
 * @brief Function pointer type for Riemann solvers.
 */
using RiemannSolver = void(*)(
    const double*, const double*, double*, const double
);

/**
 * @brief Initializes the Riemann solver.
 *
 * @param solver Selected Riemann solver.
 */
void init_riemann(Riemann solver);

/**
 * @brief Compute the Riemann flux across a face.
 *
 * This function computes the Riemann flux across a face using the selected
 * Riemann solver function.
 *
 * @param WL Pointer to the left state array.
 * @param WR Pointer to the right state array.
 * @param F  Pointer to the output flux array.
 * @param gam Specific heat ratio of the fluid.
 */
void riemann(const double* WL, const double* WR, double* F, const double gam);

} // namespace eulercpp::physics
