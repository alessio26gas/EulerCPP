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
 * @file boundaries.cpp
 * @brief Implements boundary condition initialization and application.
 *
 * This module applies physical boundary conditions to ensure proper
 * enforcement of physical constraints at domain boundaries.
 *
 * @author Alessio Improta
 */

#include <omp.h>

#include <eulercpp/input/load_bc.hpp>
#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/physics/boundaries/all_boundaries.hpp>

namespace eulercpp::physics {

/**
 * @brief Apply boundary conditions to the simulation fields.
 *
 * Iterates over all boundary faces in the mesh and applies the correct
 * boundary condition. Each condition modifies the solution fluxes at
 * the boundary.
 *
 * Parallelized with OpenMP for large meshes.
 *
 * @param sim The simulation object containing mesh, fields, and input data.
 *
 * @throws std::runtime_error if an unknown boundary condition is encountered.
 */
void apply_boundary_conditions(Simulation& sim) {
    const auto& input = sim.input;
    const auto& mesh = sim.mesh;
    auto& fields = sim.fields;

    #pragma omp parallel for
    for (int f = 0; f < mesh.n_faces; ++f) {
        const auto& face = mesh.faces[f];
        if (face.opposite != -1) continue;

        const auto& bc = input.bc.boundaries[face.flag];

        switch (bc.type) {
        case BCType::SUPERSONIC_INLET:
            bc::supersonic_inlet(input, face, f, bc, fields);
            break;

        case BCType::SUPERSONIC_OUTLET:
            bc::supersonic_outlet(input, face, f, fields);
            break;

        case BCType::STAGNATION_INLET:
            bc::stagnation_inlet(input, face, f, bc, fields);
            break;

        case BCType::SUBSONIC_INLET:
            bc::subsonic_inlet(input, face, f, bc, fields);
            break;

        case BCType::PRESSURE_OUTLET:
            bc::pressure_outlet(input, face, f, bc, fields);
            break;

        case BCType::MOVING_WALL:
        case BCType::WALL:
        case BCType::SLIPWALL:
        case BCType::SYMMETRY:
            bc::symmetry(input, face, f, fields);
            break;

        case BCType::AXIS:
            break;

        default:
            throw std::runtime_error("Unknown boundary condition type.");
        }

        double A = face.area;
        for (int v = 0; v < 5; ++v) {
            fields.F(f, v) *= A;
        }
    }
}

/**
 * @brief Initialize boundary conditions before starting the simulation.
 *
 * @param sim The simulation object containing mesh, fields, and input data.
 *
 * @throws std::runtime_error if an unknown boundary condition is encountered.
 */
void init_boundaries(Simulation& sim) {
    auto& mesh = sim.mesh;
    auto& input = sim.input;

    auto& boundaries = input.bc.boundaries;
    for (auto& bc : boundaries) {
        switch (bc.type) {
        case BCType::SUPERSONIC_INLET:
            bc::init_supersonic_inlet(bc, sim);
            break;

        case BCType::STAGNATION_INLET:
            bc::init_stagnation_inlet(bc, sim);
            break;

        case BCType::SUBSONIC_INLET:
        case BCType::MOVING_WALL:
        case BCType::SLIPWALL:
        case BCType::WALL:
        case BCType::SUPERSONIC_OUTLET:
        case BCType::PRESSURE_OUTLET:
        case BCType::AXIS:
        case BCType::SYMMETRY:
            break;

        default:
            throw std::runtime_error("Unknown boundary condition type.");
        }
    }

    #pragma omp parallel for
    for (int f = 0; f < mesh.n_faces; ++f) {
        auto& face = mesh.faces[f];
        if (face.opposite != -1) continue;
        if (0 <= face.flag && face.flag < input.bc.n_boundaries) continue;

        face.flag = 0;
        Logger::warning() << "Invalid boundary id found, defaulting to 0.";
    }
}

} // namespace eulercpp::physics
