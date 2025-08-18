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
 * @file corrections.cpp
 * @brief Implements solution corrections for unphysical values.
 *
 * Checks for NaNs, Infs, or unphysical states in the conserved variables
 * and applies local averaging corrections using neighboring cells.
 *
 * @author Alessio Improta
 */

#include <omp.h>

#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp::physics {

/**
 * @brief Apply local corrections to unphysical solution values.
 *
 * The function checks each element in the mesh for NaN/Inf values or 
 * unphysical conserved variables (negative density or pressure). If found, 
 * the solution is corrected using weighted averages of neighboring cells' 
 * previous state.
 *
 * Parallelized with OpenMP for improved performance.
 *
 * @param sim The Simulation object.
 *
 * @throws std::runtime_error if the number of corrections exceeds a threshold.
 */
void apply_corrections(Simulation& sim) {
    const auto& input = sim.input;
    const auto& mesh = sim.mesh;
    auto& fields = sim.fields;

    const int n_elements = mesh.n_elements;
    const int n_var = 5;

    int corrections = 0;  // Total number of corrections made.

    #pragma omp parallel
    {
        int thread_corrections = 0;  // Corrections made by the current thread.

        #pragma omp for
        for (int i = 0; i < n_elements; ++i) {
            bool corrected = false;

            // Check if the solution value is NaN or Inf.
            for (int v = 0; v < n_var; ++v) {
                if (std::isnan(fields.W(i, v)) || std::isinf(fields.W(i, v))) {
                    corrected = true;
                    break;
                }
            }

            // Check if the solution value is unphysical.
            double K = 0.5*(
                fields.W(i, 1)*fields.W(i, 1) +
                fields.W(i, 2)*fields.W(i, 2) +
                fields.W(i, 3)*fields.W(i, 3)
            ) / fields.W(i, 0);
            if (fields.W(i, 4) < K || fields.W(i, 0) < 0.0) {
                corrected = true;
            }

            if (corrected) {
                const auto& elem = mesh.elements[i];
                const int n_f = elem.n_faces;
                thread_corrections++;  // Increment thread's correction count.
                for (int v = 0; v < n_var; ++v) {
                    double correction = 0.0;
                    int den = 0;
                    for (int f = 0; f < n_f; ++f) {
                        int n = elem.neighbors[f];
                        if (n < 0) continue;
                        if (
                            std::isnan(fields.Wold(n, v)) ||
                            std::isinf(fields.Wold(n, v))
                        ) continue;
                        double Kn = 0.5*(
                            fields.Wold(n, 1)*fields.Wold(n, 1) +
                            fields.Wold(n, 2)*fields.Wold(n, 2) +
                            fields.Wold(n, 3)*fields.Wold(n, 3)
                        ) / fields.Wold(n, 0);
                        if (
                            fields.Wold(n, 0) < 0.0 ||
                            fields.Wold(n, 4) < Kn
                        ) continue;
                        correction += fields.Wold(n, v);
                        den++;
                    }
                    if (den == 0) {
                        for (int f = 0; f < n_f; ++f) {
                            const int n = elem.neighbors[f];
                            const auto& elem_n = mesh.elements[n];
                            const int n_fn = elem_n.n_faces;
                            if (n < 0) continue;
                            for (int fn = 0; fn < n_fn; ++fn) {
                                int nn = elem_n.neighbors[fn];
                                if (nn < 0) continue;
                                if (
                                    std::isnan(fields.Wold(nn, v)) ||
                                    std::isinf(fields.Wold(nn, v))
                                ) continue;
                                double Knn = 0.5*(
                                    fields.Wold(nn, 1)*fields.Wold(nn, 1) +
                                    fields.Wold(nn, 2)*fields.Wold(nn, 2) +
                                    fields.Wold(nn, 3)*fields.Wold(nn, 3)
                                ) / fields.Wold(nn, 0);
                                if (
                                    fields.Wold(nn, 0) < 0.0 ||
                                    fields.Wold(nn, 4) < Knn
                                ) continue;
                                correction += fields.Wold(nn, v);
                                den++;
                            }
                        }
                    }
                    fields.W(i, v) = correction / den;
                }
            }
        }

        // Update the global corrections count atomically.
        #pragma omp atomic
        corrections += thread_corrections;
    }

    // If the number of corrections exceeds the threshold, 
    // terminate the simulation.
    if (corrections > 0.1 * mesh.n_boundaries) {
        throw std::runtime_error("A floating point error has occurred.");
    }

    // Print a message indicating how many 
    // corrections were applied, if necessary.
    if (corrections > 0)
        Logger::debug() << "corrections limited on "
                        << corrections << " cells.";
}

} // namespace eulercpp::physics
