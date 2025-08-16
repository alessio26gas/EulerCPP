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
 * @file logger.hpp
 * @brief Logging utility with console and file output for EulerCPP.
 *
 * The Logger provides leveled logging streams (DEBUG, INFO, SUCCESS,
 * WARNING, ERROR, RESIDUALS) with optional colored console output and
 * timestamped file logging. Verbosity can be controlled globally.
 *
 * Residuals are handled separately and can be redirected to a dedicated
 * file (RHS log).
 *
 * @see logger.cpp
 * @author Alessio Improta
 */

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace eulercpp {

/**
 * @brief Centralized logging utility for console and file output.
 *
 * This class implements a singleton logger. Clients obtain streams via
 * static functions (e.g., `Logger::info() << "message";`).
 */
class Logger {
public:
    /**
     * @brief Logging severity levels.
     */
    enum class Level {
        RESIDUALS,   ///< Residual log.
        ERROR,      ///< Errors or fatal issues.
        WARNING,    ///< Warnings about potential issues.
        SUCCESS,    ///< Successful completion or progress.
        INFO,       ///< General information messages.
        DEBUG,      ///< Detailed debug messages.
    };

    /**
     * @brief Logging stream wrapper.
     *
     * Buffers log messages until destruction, then flushes them
     * to console and/or file with formatting (timestamp, color, etc.).
     */
    class Stream {
    public:
        /**
         * @brief Constructs a logging stream.
         * @param console Target console output stream.
         * @param file Optional file stream.
         * @param level Severity level.
         */
        Stream(std::ostream& console, std::ofstream* file, Level level);

        /**
         * @brief Destructor flushes the buffered log message.
         */
        ~Stream();

        Stream(const Stream&) = delete;
        Stream& operator=(const Stream&) = delete;
        Stream(Stream&&) noexcept = default;
        Stream& operator=(Stream&&) noexcept = delete;

        /**
         * @brief Generic stream operator to append data to log.
         */
        template<typename T>
        Stream& operator<<(const T& value) {
            if (level == Level::RESIDUALS) {
                buffer << std::scientific << std::setprecision(3) << std::setw(11) << value;
            } else {
                buffer << value;
            }
            return *this;
        }

        Stream& operator<<(std::ostream& (*manip)(std::ostream&)) {
            buffer << manip;
            return *this;
        }

    private:
        std::ostringstream buffer;
        std::ostream& console;
        std::ofstream* file;
        Level level;
    };

    static void set_log_file(const std::string& filename);
    static void set_rhs_file(const std::string& filename);
    static void set_verbosity(Level level);
    static std::string get_verbosity();

    static Stream debug();
    static Stream info();
    static Stream success();
    static Stream warning();
    static Stream error();

    static Stream residuals();

private:
    std::ofstream rhs_file;
    std::ofstream log_file;
    bool log_file_enabled = false;
    bool rhs_file_enabled = false;
    static Logger& instance();
    static Level verbosity;

    static const char* level_prefix(Level level);
    static const char* level_color(Level level);
};

} // namespace eulercpp
