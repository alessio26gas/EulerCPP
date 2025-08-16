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
    @file load_init.cpp
    @brief Loads and sets up the initial conditions for the simulation.

    This source file provides functionality to parse initial condition 
    parameters from the configuration map. It supports both a restart 
    mechanism and fresh initialization from density/temperature-based 
    states.

    Main features:
      - Restart configuration:
          - "restart"       : Flag to enable restart mode (0 = no, 1 = yes).
          - "restart_file"  : Path to the restart file (required if restart=1).
      - Initial state variables:
          - "initial_variables" : Defines whether initialization is based on 
                                  TEMPERATURE (0) or DENSITY (1).
          - "rho_0", "p_0", "T_0", "u_0", "v_0", "w_0"
      - Additional blocks:
          - "additional_blocks" : Number of user-defined initialization blocks.
          - Each block b uses keys with suffix `_b` (1-based index):
              - rho_b, p_b, T_b, u_b, v_b, w_b
              - xmin_b, xmax_b, ymin_b, ymax_b, zmin_b, zmax_b

    Updates both the global initial state `W0` and optional per-block states.

    @see input.hpp, input_helpers.hpp, load_init.hpp
    @author  Alessio Improta
 */

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <cmath>

#include <eulercpp/input/input.hpp>
#include <eulercpp/input/input_helpers.hpp>
#include <eulercpp/input/load_init.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp {

/**
 * @brief Load initial condition settings from configuration map.
 *
 * This function configures the simulation's initial state in two modes:
 * - **Restart mode**: Loads from a provided restart file.
 * - **Fresh initialization**: Builds initial conditions based on
 *   pressure/density/temperature and velocity inputs.
 *
 * Supports additional initialization blocks with user-defined
 * physical variables and spatial extents.
 *
 * @param config Key-value map of configuration entries.
 * @param input  Input structure to populate with initialization data.
 *
 * @throw std::invalid_argument If `restart=1` but no file is given, or
 *        if an invalid `initial_variables` mode is specified.
 */
void load_init(const std::map<std::string, std::string>& config, Input& input) {
    auto it = config.find("restart");
    if (it != config.end())
        input.init.restart = std::stoi(it->second);

    if (input.init.restart == 1) {
        it = config.find("restart_file");
        if (it != config.end()) {
            input.init.restart_file = it->second;
            return;
        }
        throw std::invalid_argument("Restart file path not found.");
    }

    it = config.find("initial_variables");
    if (it != config.end())
        input.init.initial_variables = static_cast<InitialVariables>(
            std::stoi(it->second)
        );

    int n_blocks = 0;
    it = config.find("additional_blocks");
    if (it != config.end())
        n_blocks = std::stoi(it->second);
    input.init.n_blocks = n_blocks;

    /// Default reference state
    double rho0 = 1.0, p0 = 101325.0, T0 = 300.0, u0 = 0.0, v0 = 0.0, w0 = 0.0;
    if (auto it = config.find("rho_0"); it != config.end())
        rho0 = std::stod(it->second);
    if (auto it = config.find("p_0"); it != config.end())
        p0   = std::stod(it->second);
    if (auto it = config.find("T_0"); it != config.end())
        T0   = std::stod(it->second);
    if (auto it = config.find("u_0"); it != config.end())
        u0   = std::stod(it->second);
    if (auto it = config.find("v_0"); it != config.end())
        v0   = std::stod(it->second);
    if (auto it = config.find("w_0"); it != config.end())
        w0   = std::stod(it->second);

    double R = input.fluid.R;
    double gamma = input.fluid.gamma;

    /// Compute reference W0
    if (input.init.initial_variables==InitialVariables::TEMPERATURE_BASED) {
        rho0 = p0 / T0 / R;
        input.init.W0[0] = rho0;
    } else if (input.init.initial_variables==InitialVariables::DENSITY_BASED) {
        T0 = p0 / rho0 / R;
        input.init.W0[0] = rho0;
    } else {
        throw std::invalid_argument("Invalid 'initial_variables' value.");
    }

    input.init.W0[1] = rho0 * u0;
    input.init.W0[2] = rho0 * v0;
    input.init.W0[3] = rho0 * w0;
    input.init.W0[4] = p0 / (gamma-1.0) + 0.5 * rho0 * (u0*u0 + v0*v0 + w0*w0);

    /// Setup additional blocks
    input.init.blocks.resize(n_blocks);
    for (int b = 0; b < n_blocks; ++b) {
        auto& blk = input.init.blocks[b];

        auto key_for_block = [&](const std::string& prefix) {
            return prefix + "_" + std::to_string(b + 1);
        };

        if (auto it = config.find(key_for_block("rho")); it != config.end())
            rho0 = std::stod(it->second);
        if (auto it = config.find(key_for_block("p")); it != config.end())
            p0   = std::stod(it->second);
        if (auto it = config.find(key_for_block("T")); it != config.end())
            T0   = std::stod(it->second);
        if (auto it = config.find(key_for_block("u")); it != config.end())
            u0   = std::stod(it->second);
        if (auto it = config.find(key_for_block("v")); it != config.end())
            v0   = std::stod(it->second);
        if (auto it = config.find(key_for_block("w")); it != config.end())
            w0   = std::stod(it->second);

        if (auto it = config.find(key_for_block("xmin")); it != config.end())
            blk.xmin = std::stod(it->second);
        if (auto it = config.find(key_for_block("xmax")); it != config.end())
            blk.xmax = std::stod(it->second);
        if (auto it = config.find(key_for_block("ymin")); it != config.end())
            blk.ymin = std::stod(it->second);
        if (auto it = config.find(key_for_block("ymax")); it != config.end())
            blk.ymax = std::stod(it->second);
        if (auto it = config.find(key_for_block("zmin")); it != config.end())
            blk.zmin = std::stod(it->second);
        if (auto it = config.find(key_for_block("zmax")); it != config.end())
            blk.zmax = std::stod(it->second);

        if (
            input.init.initial_variables == InitialVariables::TEMPERATURE_BASED
        ) {
            rho0 = p0 / T0 / R;
            blk.W0[0] = rho0;
        } else if (
            input.init.initial_variables == InitialVariables::DENSITY_BASED
        ) {
            T0 = p0 / rho0 / R;
            blk.W0[0] = rho0;
        } else {
            throw std::invalid_argument("Invalid 'initial_variables' value.");
        }

        blk.W0[1] = rho0 * u0;
        blk.W0[2] = rho0 * v0;
        blk.W0[3] = rho0 * w0;
        blk.W0[4] = p0 / (gamma-1.0) + 0.5 * rho0 * (u0*u0 + v0*v0 + w0*w0);
    }
}

} // namespace eulercpp
