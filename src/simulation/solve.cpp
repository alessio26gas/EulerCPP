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
 * @file solve.cpp
 * @brief Implements the main solver loop for EulerCPP simulations.
 *
 * This file contains the implementation of the `solve` function,
 * which advances the simulation in time according to the configured
 * numerical scheme. It handles:
 * - Multi-stage time integration
 * - Gradient computation and reconstruction
 * - Flux evaluation and boundary condition application
 * - Solution update and corrections
 * - Output and restart handling
 * - Logging of residuals and simulation status
 *
 * @author Alessio Improta
 */

#include <ctime>
#include <iostream>

#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/simulation/solve.hpp>
#include <eulercpp/output/logger.hpp>
#include <eulercpp/output/writer.hpp>
#include <eulercpp/physics/boundaries.hpp>
#include <eulercpp/physics/corrections.hpp>
#include <eulercpp/physics/fluxes.hpp>
#include <eulercpp/physics/sources.hpp>
#include <eulercpp/physics/timestep.hpp>
#include <eulercpp/math/gradients.hpp>
#include <eulercpp/math/reconstruction.hpp>
#include <eulercpp/math/solution_update.hpp>
#include <eulercpp/math/time_utils.hpp>

namespace eulercpp {

/**
 * @brief Perform the main time-stepping solver loop.
 *
 * This function advances the simulation by iteratively updating the solution
 * fields. It performs the following steps in each iteration:
 * - Prepare solution for update
 * - Update timestep
 * - Update source terms
 * - Compute gradients and reconstructions
 * - Compute fluxes and apply boundary conditions
 * - Advance solution in time
 * - Apply physical corrections
 * - Print residuals and save output periodically
 *
 * The solver respects maximum iteration count, maximum simulation time,
 * and allows for early stopping via signal handling.
 *
 * @param sim Reference to the `Simulation` object to solve.
 */
void solve(Simulation& sim) {
    clock_t start = clock();

    const Input& input = sim.input;
    Fields& fields     = sim.fields;
    Status& status     = sim.status;
    int&    iter       = status.iteration;
    double& time       = status.time;
    bool&   stopped    = status.stopped;

    while (iter < input.numerical.maxiter &&
           time < input.numerical.maxtime &&
           !stopped) {
        iter++;
        fields.prepare_solution_update();

        physics::update_timestep(sim);
        physics::update_sources(sim);

        for (int stage = 0; stage < input.numerical.time_stages; ++stage) {
            math::compute_gradients(sim);
            math::reconstruction(sim);

            physics::compute_fluxes(sim);
            physics::apply_boundary_conditions(sim);

            math::advance_solution(sim);

            physics::apply_corrections(sim);
        }

        if ((iter - 1) % input.output.prints_info_delay == 0) {
            auto s = Logger::residuals();
            s << "iter" << "time";
            for (int v = 0; v < 5; ++v) {
                s << ("rhs" + std::to_string(v));
            }
        }

        if (iter % input.output.prints_delay == 0) {
            auto s = Logger::residuals();
            const std::array<double, 5> residuals = fields.get_residuals();
            s << iter << status.time;
            for (int v = 0; v < 5; ++v) {
                s << residuals[v];
            }
        }

        if (iter % input.output.probe_delay == 0)
            Writer::save_probes(sim);

        if (iter % input.output.report_delay == 0)
            Writer::save_reports(sim);

        if (iter % input.output.output_delay == 0)
            Writer::save_solution(sim);

        if (iter % input.output.restart_delay == 0)
            Writer::save_restart(sim);
    }

    if (iter == input.numerical.maxiter) {
        Logger::info() << "Maximum number of iterations ("
                       << iter << ") reached.";
    }

    if (time >= input.numerical.maxtime) {
        Logger::info() << "Maximum simulation time ("
                       << math::format_duration(input.numerical.maxtime)
                       << ") reached.";
    }

    if (stopped) {
        Logger::warning() << "The simulation has been interrupted.";
    }

    Writer::save_solution(sim);
    Writer::save_restart(sim);

    Writer::probes_stream.close();
    Writer::reports_stream.close();

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    Logger::success() << "Simulation complete. ("
                      << math::format_duration(elapsed) << ")";
}

} // namespace eulercpp
