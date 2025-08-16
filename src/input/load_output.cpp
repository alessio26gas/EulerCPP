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
    @file load_output.cpp
    @brief Loads output-related parameters from the configuration file.

    This source file contains the function responsible for extracting
    output settings from a key-value configuration map.

    Expected keys:
      - "output_format"      : Output file format identifier.
      - "output_delay"       : Iterations between writing output files.
      - "prints_delay"       : Iterations between console prints.
      - "prints_info_delay"  : Iterations between info header prints.
      - "restart_delay"      : Iterations between restart file saves.
      - "output_folder"      : Path to the folder where output files are saved.
      - "output_name"        : Base name of the output files.

    Updates the global Input structure with these values.

    @see input.hpp, input_helpers.hpp, load_output.hpp
    @author Alessio Improta
 */

#include <string>
#include <map>

#include <eulercpp/input/input.hpp>
#include <eulercpp/input/input_helpers.hpp>
#include <eulercpp/input/load_output.hpp>

namespace eulercpp {

/**
 * @brief Populate output parameters from a configuration map.
 *
 * Reads output format, delays for writing output, console prints,
 * restart files, and output folder/name settings.
 *
 * @param config Map of configuration key-value pairs.
 * @param input  Input structure to update with output parameters.
 */
void load_output(
    const std::map<std::string, std::string>& config, Input& input
) {
    auto it = config.find("output_format");
    if (it != config.end())
        input.output.output_format = std::stoi(it->second);

    it = config.find("output_delay");
    if (it != config.end())
        input.output.output_delay = std::stoi(it->second);

    it = config.find("prints_delay");
    if (it != config.end())
        input.output.prints_delay = std::stoi(it->second);

    it = config.find("prints_info_delay");
    if (it != config.end())
        input.output.prints_info_delay = std::stoi(it->second);

    it = config.find("restart_delay");
    if (it != config.end())
        input.output.restart_delay = std::stoi(it->second);

    it = config.find("output_folder");
    if (it != config.end())
        input.output.output_folder = it->second;

    it = config.find("output_name");
    if (it != config.end())
        input.output.output_name = it->second;
}

} // namespace eulercpp
