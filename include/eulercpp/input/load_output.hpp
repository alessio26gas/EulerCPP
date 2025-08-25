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
 * @file load_output.hpp
 * @brief Declares structures and functions for simulation output settings.
 *
 * Defines OutputSettings, which holds parameters for output behavior
 * such as file format, delays, folder paths, and base names.
 * Declares load_output() to load these settings from a config map
 * into the global Input structure.
 *
 * @author Alessio Improta
 */

#pragma once

#include <map>
#include <string>

namespace eulercpp {

struct Input;

/**
 * @struct Probe
 * @brief Defines a probe point in the domain for monitoring flow variables.
 *
 * A probe is associated with the nearest mesh element centroid to the 
 * user-defined location. During the simulation, primitive/conserved variables 
 * at that element are sampled and written to the probe output file.
 */
struct Probe {
    int element = 0; /**< Index of the element assigned to this probe. */
    std::array<double, 3> location = {0.0}; /**< User-defined probe location. */
};

/**
 * @struct Report
 * @brief Defines a global report over a boundary patch.
 *
 * A report computes integral quantities (mass flow, forces, and moments) over
 * all faces belonging to a specified boundary tag. Moments are computed with
 * respect to the given center of gravity.
 */
struct Report {
    int boundary = 0; /**< Boundary tag associated with this report. */
    std::array<double, 3> cg = {0.0}; /**< Reference center of gravity. */
};

/**
 * @struct OutputSettings
 * @brief Holds all the output settings.
 */
struct OutputSettings {
    int output_format = 0;       /**< Output file format. */
    int output_delay = 1;        /**< Output file write dela. */
    int prints_delay = 1;        /**< Terminal output print delay. */
    int prints_info_delay = 0;   /**< Detailed info print delay. */
    int restart_delay = 1;       /**< Restart file write delay. */
    int restart_format = 0;      /**< Restart file format. */
    std::string output_folder = "output"; /**< Output folder path. */
    std::string output_name = "output";   /**< Base name for output files. */

    int probe_delay = 1;          /**< Interval between writing probe data. */
    int n_probes = 0;             /**< Number of probes. */
    std::vector<Probe> probes;    /**< Collection of probes. */

    int report_delay = 1;         /**< Interval between writing report data. */
    int n_reports = 0;            /**< Number of reports. */
    std::vector<Report> reports;  /**< Collection of reports. */
};

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
);

} // namespace eulercpp
