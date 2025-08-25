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
 * @file writer.hpp
 * @brief Handles simulation output writing, including VTK and restart files.
 *
 * This header defines the Writer class, which provides methods for
 * saving simulation data in VTK format (binary or ASCII) and restart files.
 *
 * @author Alessio Improta
 */

#pragma once

#include <string>
#include <fstream>

#include <eulercpp/simulation/simulation.hpp>

namespace eulercpp {

/**
 * @class Writer
 * @brief Handles writing simulation data to disk.
 *
 * The Writer class centralizes all I/O for the simulation, including
 * field output, restart files, and post-processing data such as probes
 * and reports. All methods are static; no instantiation is required.
 */
class Writer {
public:
    /**
     * @enum Format
     * @brief Supported solution output formats.
     */
    enum class Format {
        VTK_BIN,   /**< Binary VTK (.vtk) */
        VTK_ASCII, /**< ASCII VTK (.vtk) */
        CSV        /**< Comma-separated values (.csv) */
    };

    /**
     * @enum RestartFormat
     * @brief Supported restart file formats.
     */
    enum class RestartFormat {
        BIN,   /**< Binary restart file */
        ASCII, /**< ASCII restart file */
    };

    /**
     * @brief Configure the output writer.
     *
     * Sets output format, directory, and base filename.
     * Creates the output directory if it does not exist.
     *
     * @param fmt Output format
     * @param restart_fmt Restart file format
     * @param output_dir Directory to save output files
     * @param output_name Base name for output files
     */
    static void configure(
        int fmt, int restart_fmt,
        const std::string& output_dir,
        const std::string& output_name
    );

    /**
     * @brief Save the current simulation state to a VTK or CSV file.
     *
     * Filename is generated as `output_name_XXXXXX`, where XXXXXX is 
     * the zero-padded iteration number.
     *
     * @param sim Simulation object to write
     *
     * @throws std::runtime_error if format is unsupported
     */
    static void save_solution(const Simulation& sim);

    /**
     * @brief Save a restart file for the current simulation state.
     *
     * The restart file is saved as `output_name.restart` in the 
     * output directory.
     *
     * @param sim Simulation object to write
     */
    static void save_restart(const Simulation& sim);

    /**
     * @brief Initialize probe output.
     *
     * Creates and opens the probe CSV file in the output directory, 
     * writes the header line, and assigns each probe to its nearest 
     * mesh element centroid.
     *
     * @param sim Simulation object
     */
    static void init_probes(Simulation& sim);

    /**
     * @brief Write probe data to the probes CSV file.
     *
     * Writes solution values at each defined probe location.
     *
     * @param sim Simulation object
     */
    static void save_probes(const Simulation& sim);

    /**
     * @brief Initialize report output.
     *
     * Creates and opens the reports CSV file in the output directory,
     * and writes the header line.
     *
     * @param sim Simulation object
     */
    static void init_reports(const Simulation& sim);

    /**
     * @brief Write report data to the reports CSV file.
     *
     * Computes integral quantities over each boundary report defined in 
     * the input and appends results to the file.
     *
     * @param sim Simulation object
     */
    static void save_reports(const Simulation& sim);

    /**
     * @brief File stream for writing probe data.
     */
    static std::ofstream probes_stream;

    /**
     * @brief File stream for writing report data.
     */
    static std::ofstream reports_stream;

private:
    Writer() = delete;  /**< Deleted constructor: Writer is a static class. */
    ~Writer() = delete; /**< Deleted destructor: Writer is a static class. */

    static Format format_;                  /**< Selected output format. */
    static RestartFormat restart_format_;   /**< Selected restart format. */
    static std::string output_dir_;         /**< Output directory path. */
    static std::string output_name_;        /**< Base name for output files. */
};

} // namespace eulercpp
