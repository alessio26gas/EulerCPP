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
 * @file riemann.cpp
 * @brief Implements the Riemann solver for Euler equations.
 *
 * Computes fluxes across a face given left and right states using
 * an approximate 1D Riemann solver. Handles shocks, rarefactions,
 * and contact discontinuities along the face-normal direction.
 * Ensures positive pressures and avoids numerical instabilities.
 *
 * @author Alessio Improta
 */

#include <array>
#include <cmath>
#include <iostream>

#include <eulercpp/physics/riemann.hpp>

namespace eulercpp::physics {

/** @brief Internal pointer to the current Riemann solver */
static RiemannSolver riemann_solver = nullptr;

/**
 * @brief Rusanov Riemann solver
 *
 * This function computes the flux at the interface between two states using 
 * the Rusanov Riemann solver. The Rusanov solver uses the maximum wave speed 
 * to estimate the flux at the interface.
 *
 * @param WL Pointer to the left state array.
 * @param WR Pointer to the right state array.
 * @param F  Pointer to the output flux array.
 * @param gam Specific heat ratio of the fluid.
 */
static inline void
rusanov(const double* WL, const double* WR, double* F, const double gam) {
    double rhoL = WL[0],       rhoR = WR[0];
    double unL  = WL[1]/rhoL,   unR = WR[1]/rhoR;
    double ut1L = WL[2]/rhoL,  ut1R = WR[2]/rhoR;
    double ut2L = WL[3]/rhoL,  ut2R = WR[3]/rhoR;
    double EL   = WL[4],         ER = WR[4];

    double pL = (gam-1.0)*(EL-0.5*rhoL*(unL*unL+ut1L*ut1L+ut2L*ut2L));
    double pR = (gam-1.0)*(ER-0.5*rhoR*(unR*unR+ut1R*ut1R+ut2R*ut2R));
    if (pL < 0) pL = 1.0e-14;
    if (pR < 0) pR = 1.0e-14;
    double aL = std::sqrt(gam * pL / rhoL);
    double aR = std::sqrt(gam * pR / rhoR);

    double FL[5] = {
        rhoL * unL,
        rhoL * unL * unL + pL,
        rhoL * unL * ut1L,
        rhoL * unL * ut2L,
        (EL + pL) * unL
    };

    double FR[5] = {
        rhoR * unR,
        rhoR * unR * unR + pR,
        rhoR * unR * ut1R,
        rhoR * unR * ut2R,
        (ER + pR) * unR
    };

    double S = std::max(std::abs(unL) + aL, std::abs(unR) + aR);

    for (int v = 0; v < 5; ++v) {
        F[v] = 0.5 * (FL[v] + FR[v]) - 0.5 * S * (WR[v] - WL[v]);
    }
}

/**
 * @brief HLL Riemann solver
 *
 * This function computes the flux at the interface between two states using 
 * the HLL (Harten-Lax-van Leer) solver. The HLL solver is a Godunov-type 
 * method which approximates the flux based on the wave speeds.
 *
 * @param WL Pointer to the left state array.
 * @param WR Pointer to the right state array.
 * @param F  Pointer to the output flux array.
 * @param gam Specific heat ratio of the fluid.
 */
static inline void
hll(const double* WL, const double* WR, double* F, const double gam) {
    double rhoL = WL[0],       rhoR = WR[0];
    double unL  = WL[1]/rhoL,   unR = WR[1]/rhoR;
    double ut1L = WL[2]/rhoL,  ut1R = WR[2]/rhoR;
    double ut2L = WL[3]/rhoL,  ut2R = WR[3]/rhoR;
    double EL   = WL[4],         ER = WR[4];

    double pL = (gam-1.0)*(EL-0.5*rhoL*(unL*unL+ut1L*ut1L+ut2L*ut2L));
    double pR = (gam-1.0)*(ER-0.5*rhoR*(unR*unR+ut1R*ut1R+ut2R*ut2R));
    if (pL < 0) pL = 1.0e-14;
    if (pR < 0) pR = 1.0e-14;
    double aL = std::sqrt(gam * pL / rhoL);
    double aR = std::sqrt(gam * pR / rhoR);

    double FL[5] = {
        rhoL * unL,
        rhoL * unL * unL + pL,
        rhoL * unL * ut1L,
        rhoL * unL * ut2L,
        (EL + pL) * unL
    };

    double FR[5] = {
        rhoR * unR,
        rhoR * unR * unR + pR,
        rhoR * unR * ut1R,
        rhoR * unR * ut2R,
        (ER + pR) * unR
    };

    double SL = std::min(unL, unR) - std::max(aL, aR);
    double SR = std::max(unL, unR) + std::max(aL, aR);

    if (SL > 0.0) {
        for (int v = 0; v < 5; ++v) {
            F[v] = FL[v];
        }
    } else if (SR < 0.0) {
        for (int v = 0; v < 5; ++v) {
            F[v] = FR[v];
        }
    } else {
        const double dS = SR - SL;
        for (int v = 0; v < 5; ++v) {
            F[v] = ((SR*FL[v]-SL*FR[v]) + SL*SR*(WR[v]-WL[v])) / dS;
        }
    }
}

/**
 * @brief HLLC Riemann solver
 *
 * This function computes the flux at the interface between two states using 
 * the HLLC (Harten-Lax-van Leer-Contact) solver. The HLLC solver improves on 
 * the HLL by including a contact wave, leading to a more accurate flux 
 * calculation.
 *
 * @param WL Pointer to the left state array.
 * @param WR Pointer to the right state array.
 * @param F  Pointer to the output flux array.
 * @param gam Specific heat ratio of the fluid.
 */
static inline void
hllc(const double* WL, const double* WR, double* F, const double gam) {
    double rhoL = WL[0],       rhoR = WR[0];
    double unL  = WL[1]/rhoL,   unR = WR[1]/rhoR;
    double ut1L = WL[2]/rhoL,  ut1R = WR[2]/rhoR;
    double ut2L = WL[3]/rhoL,  ut2R = WR[3]/rhoR;
    double EL   = WL[4],         ER = WR[4];

    double pL = (gam-1.0)*(EL-0.5*rhoL*(unL*unL+ut1L*ut1L+ut2L*ut2L));
    double pR = (gam-1.0)*(ER-0.5*rhoR*(unR*unR+ut1R*ut1R+ut2R*ut2R));
    if (pL < 0) pL = 1.0e-14;
    if (pR < 0) pR = 1.0e-14;
    double aL = std::sqrt(gam * pL / rhoL);
    double aR = std::sqrt(gam * pR / rhoR);

    double FL[5] = {
        rhoL * unL,
        rhoL * unL * unL + pL,
        rhoL * unL * ut1L,
        rhoL * unL * ut2L,
        (EL + pL) * unL
    };

    double FR[5] = {
        rhoR * unR,
        rhoR * unR * unR + pR,
        rhoR * unR * ut1R,
        rhoR * unR * ut2R,
        (ER + pR) * unR
    };

    double SL = std::min(unL, unR) - std::max(aL, aR);
    double SR = std::max(unL, unR) + std::max(aL, aR);

    if (SL > 0.0) {
        for (int v = 0; v < 5; ++v) {
            F[v] = FL[v];
        }
    } else if (SR < 0.0) {
        for (int v = 0; v < 5; ++v) {
            F[v] = FR[v];
        }
    } else {
        double SM = (pR - pL + WL[1]*(SL - unL) - WR[1]*(SR - unR))
                    / (rhoL * (SL - unL) - rhoR * (SR - unR));
        double pM = 0.5 * (
            pL + pR + rhoL*(SL-unL)*(SM-unL) + rhoR*(SR-unR)*(SM-unR)
        );
        double D[] = {0.0, 1.0, 0.0, 0.0, SM};

        if (SM > 0.0) {
            const double dSL = SL - SM;
            for (int v = 0; v < 5; ++v) {
                F[v] = (SM * (SL * WL[v] - FL[v]) + SL * pM * D[v]) / dSL;
            }
        } else {
            const double dSR = SR - SM;
            for (int v = 0; v < 5; ++v) {
                F[v] = (SM * (SR * WR[v] - FR[v]) + SR * pM * D[v]) / dSR;
            }
        }
    }
}

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
void riemann(const double* WL, const double* WR, double* F, const double gam) {
    riemann_solver(WL, WR, F, gam);
}

/**
 * @brief Initializes the Riemann solver.
 *
 * @param solver Selected Riemann solver.
 */
void init_riemann(Riemann solver) {
    switch (solver) {
        case Riemann::RUSANOV:  riemann_solver = rusanov;   break;
        case Riemann::HLL:      riemann_solver = hll;       break;
        case Riemann::HLLC:     riemann_solver = hllc;      break;
        default: throw std::invalid_argument("Unknown Riemann solver.");
    }
}

} // namespace eulercpp::physics
