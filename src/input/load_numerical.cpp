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
 * @file load_numerical.cpp
 * @brief Loads numerical parameters from the simulation configuration file.
 *
 * This source file contains the function responsible for extracting 
 * numerical method parameters from a key-value configuration map.
 *
 * Expected keys:
 *  - "time_stages"    : Number of stages in the time integration method.
 *  - "a"              : Time integration coefficients (comma-separated).
 *  - "CFL"            : CFL number for the numerical scheme.
 *  - "maxtime"        : Maximum simulation time.
 *  - "maxiter"        : Maximum number of iterations.
 *  - "reconstruction" : Reconstruction method identifier.
 *  - "limiter"        : Slope limiter identifier.
 *
 * Checks are performed to ensure the number of coefficients matches 
 * the number of stages.
 *
 * @see input.hpp, input_helpers.hpp, load_numerical.hpp
 * @see math::Reconstruction, math::Limiter
 * @author Alessio Improta
 */

#include <string>
#include <map>
#include <stdexcept>

#include <eulercpp/input/input.hpp>
#include <eulercpp/input/input_helpers.hpp>
#include <eulercpp/input/load_numerical.hpp>
#include <eulercpp/math/limiters.hpp>
#include <eulercpp/math/reconstruction.hpp>
#include <eulercpp/physics/riemann.hpp>

namespace eulercpp {

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
void load_numerical(
    const std::map<std::string, std::string>& config, Input& input
) {
    auto it = config.find("time_stages");
    if (it != config.end())
        input.numerical.time_stages = std::stoi(it->second);

    it = config.find("a");
    if (it != config.end())
        input.numerical.a = parse_vector(it->second);

    it = config.find("CFL");
    if (it != config.end())
        input.numerical.CFL = std::stod(it->second);

    it = config.find("maxtime");
    if (it != config.end())
        input.numerical.maxtime = std::stod(it->second);

    it = config.find("maxiter");
    if (it != config.end())
        input.numerical.maxiter = std::stoi(it->second);

    it = config.find("reconstruction");
    if (it != config.end())
        input.numerical.reconstruction = static_cast<math::Reconstruction>(
            std::stoi(it->second)
        );

    it = config.find("limiter");
    if (it != config.end())
        input.numerical.limiter = static_cast<math::Limiter>(
            std::stoi(it->second)
        );

    it = config.find("riemann");
    if (it != config.end())
        input.numerical.riemann = static_cast<physics::Riemann>(
            std::stoi(it->second)
        );

    if (input.numerical.time_stages == 1) {
        input.numerical.a.clear();
        input.numerical.a.push_back(1.0);
    } else if (input.numerical.time_stages > 1) {
        if (input.numerical.a.size() != input.numerical.time_stages) {
            throw std::invalid_argument(
                "Number of coefficients does "
                "not match the number of time stages."
            );
        }
    } else {
        throw std::invalid_argument("Invalid number of time stages.");
    }
}

} // namespace eulercpp
