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
 * @file axisymmetric.cpp
 * @brief Implements axisymmetric mode.
 *
 * This file provides the implementation of axisymmetric source term 
 * computation and mesh initializations for axisymmetric mode.
 *
 * All element loops are parallelized using OpenMP.
 *
 * @author Alessio Improta
 */

#include <omp.h>

#include <eulercpp/simulation/simulation.hpp>

namespace eulercpp::physics {

/**
 * @brief Apply axisymmetric source terms to the simulation.
 *
 * Computes additional source contributions arising from axisymmetric mode.
 *
 * @param sim Reference to the Simulation object to be updated.
 */
void axisymmetry_sources(Simulation& sim) {
    const auto& mesh = sim.mesh;
    const auto& input = sim.input;
    auto& fields = sim.fields;

    #pragma omp parallel for
    for (int i = 0; i < mesh.n_elements; ++i) {
        const double gam = input.fluid.gamma;
        const double rho = fields.W(i, 0);
        const double rhoV2 = (
            fields.W(i, 1)*fields.W(i, 1)
            +fields.W(i, 2)*fields.W(i, 2)
            +fields.W(i, 3)*fields.W(i, 3)
        )/fields.W(i, 0);
        const double E = fields.W(i, 4);
        double p = (gam-1.0)*(E-0.5*rhoV2);
        if (p < 0.0) p = 1.0e-14;
        fields.S(i, 2) += p / mesh.elements[i].centroid[1];
    }
}

/**
 * @brief Initialize the simulation mesh for axisymmetric calculations.
 *
 * Modifies the volume of each element and the area of each face by 
 * multiplying with the radial coordinate to account for axisymmetry.
 *
 * @param sim Reference to the Simulation object to be initialized.
 */
void init_axisymmetry(Simulation& sim) {
    auto& mesh = sim.mesh;

    #pragma omp parallel for
    for (int i = 0; i < mesh.n_elements; ++i) {
        mesh.elements[i].volume *= mesh.elements[i].centroid[1];
    }

    #pragma omp parallel for
    for (int f = 0; f < mesh.n_faces; ++f) {
        mesh.faces[f].area *= mesh.faces[f].centroid[1];
    }
}

} // namespace eulercpp::physics
