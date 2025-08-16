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
#include <eulercpp/simulation/simulation.hpp>

namespace eulercpp {

/**
 * @brief Handles writing simulation data to disk.
 */
class Writer {
public:
    /**
     * @brief Supported output formats
     */
    enum class Format {
        VTK_BIN,   /**< Binary VTK */
        VTK_ASCII  /**< ASCII VTK */
    };

    /**
     * @brief Configure the output writer.
     *
     * Sets output format, directory, and base filename.
     * Creates the output directory if it does not exist.
     *
     * @param fmt Output format
     * @param output_dir Directory to save output files
     * @param output_name Base name for output files
     */
    static void configure(
        int fmt,
        const std::string& output_dir,
        const std::string& output_name
    );

    /**
     * @brief Save the current simulation state to a VTK file.
     *
     * Filename is generated as `output_name_XXXXXX`, where XXXXXX is 
     * the zero-padded iteration number.
     *
     * @param sim Simulation object to write
     *
     * @throws std::runtime_error if format is unsupported
     */
    static void save(const Simulation& sim);

    /**
     * @brief Save a restart file for the current simulation state.
     *
     * The restart file is saved as `output_name.restart` in the 
     * output directory.
     *
     * @param sim Simulation object to write
     */
    static void save_restart(const Simulation& sim);

private:
    Writer() = delete;
    ~Writer() = delete;

    static Format format_;
    static std::string output_dir_;
    static std::string output_name_;
};

} // namespace eulercpp
