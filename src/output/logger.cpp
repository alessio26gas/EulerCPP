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
 * @file logger.cpp
 * @brief Implementation of the logging utility for eulercpp.
 *
 * This file defines the Logger class functions for generating
 * leveled console and file logs with optional color formatting.
 *
 * ## Features
 * - **Severity levels**: DEBUG, INFO, SUCCESS, WARNING, ERROR, RESIDUALS.
 * - **Console output** with ANSI colors and timestamps.
 * - **File logging** with identical format (no colors).
 * - **Residuals** logging redirected optionally to a separate RHS file.
 * - **Singleton design pattern** ensures a single logger instance.
 *
 * ## Usage example
 * @code
 * using namespace eulercpp;
 *
 * Logger::set_log_file("run.log");
 * Logger::set_rhs_file("run.rhs");
 * Logger::set_verbosity(Logger::Level::INFO);
 *
 * Logger::info() << "Hello!";
 * @endcode
 *
 * @see logger.hpp
 * @author Alessio Improta
 */

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <eulercpp/output/logger.hpp>

namespace eulercpp {

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

Logger::Level Logger::verbosity = Logger::Level::INFO;

Logger::Stream Logger::debug() {
    Logger& logger = Logger::instance();
    return Stream(
        std::cout,
        logger.log_file_enabled ? &logger.log_file : nullptr,
        Level::DEBUG
    );
}

Logger::Stream Logger::info() {
    Logger& logger = Logger::instance();
    return Stream(
        std::cout,
        logger.log_file_enabled ? &logger.log_file : nullptr,
        Level::INFO
    );
}

Logger::Stream Logger::success() {
    Logger& logger = Logger::instance();
    return Stream(
        std::cout,
        logger.log_file_enabled ? &logger.log_file : nullptr,
        Level::SUCCESS
    );
}

Logger::Stream Logger::warning() {
    Logger& logger = Logger::instance();
    return Stream(
        std::cout,
        logger.log_file_enabled ? &logger.log_file : nullptr,
        Level::WARNING
    );
}

Logger::Stream Logger::error() {
    Logger& logger = Logger::instance();
    return Stream(
        std::cerr,
        logger.log_file_enabled ? &logger.log_file : nullptr,
        Level::ERROR
    );
}

Logger::Stream Logger::residuals() {
    Logger& logger = Logger::instance();
    return Stream(
        std::cout,
        logger.rhs_file_enabled ? &logger.rhs_file : nullptr,
        Level::RESIDUALS
    );
}

Logger::Stream::Stream(std::ostream& console, std::ofstream* file, Level level)
    : console(console), file(file), level(level) {}

Logger::Stream::~Stream() {
    if (level > Logger::verbosity) {
        return;
    }

    if (level == Level::RESIDUALS) {
        if (file) {
            *file << buffer.str() << '\n';
            file->flush();
        } else {
            std::ostringstream formatted;
            formatted << buffer.str() << '\n';
            console << formatted.str();
        }
        return;
    }

    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    std::tm now_tm;
#if defined(_MSC_VER) || defined(__MINGW32__)
    localtime_s(&now_tm, &now_time_t);  // Windows
#else
    localtime_r(&now_time_t, &now_tm);  // POSIX
#endif

    std::ostringstream time_stream;
    time_stream << std::put_time(&now_tm, "%H:%M:%S");

    const char* reset = "\033[0m";
    const char* prefix = Logger::level_prefix(level);
    const char* color = Logger::level_color(level);

    std::ostringstream formatted;
    formatted << color << "[" << time_stream.str() << "]"
              << std::setw(10) << std::left << prefix
              << buffer.str() << reset << '\n';
    console << formatted.str();

    if (file) {
        *file << "[" << time_stream.str() << "]"
              << std::setw(10) << std::left << prefix
              << buffer.str() << '\n';
        file->flush();
    }
}

const char* Logger::level_prefix(Level level) {
    switch (level) {
        case Level::INFO:       return "[INFO]";
        case Level::SUCCESS:    return "[SUCCESS]";
        case Level::WARNING:    return "[WARNING]";
        case Level::ERROR:      return "[ERROR]";
        case Level::RESIDUALS:  return "";
        default: return "[DEBUG]";
    }
}

const char* Logger::level_color(Level level) {
    switch (level) {
        case Level::SUCCESS: return "\033[92m"; // Green
        case Level::WARNING: return "\033[93m"; // Yellow
        case Level::ERROR:   return "\033[91m"; // Red
        default: return "\033[0m"; // Reset
    }
}

std::string Logger::get_verbosity() {
    switch (verbosity) {
        case Level::INFO:       return "INFO";
        case Level::SUCCESS:    return "SUCCESS";
        case Level::WARNING:    return "WARNING";
        case Level::ERROR:      return "ERROR";
        case Level::RESIDUALS:  return "SILENT";
        default: return "DEBUG";
    }
}

void Logger::set_log_file(const std::string& filename) {
    Logger& logger = Logger::instance();
    logger.log_file.open(filename);
    logger.log_file_enabled = logger.log_file.is_open();
}

void Logger::set_rhs_file(const std::string& filename) {
    Logger& logger = Logger::instance();
    logger.rhs_file.open(filename);
    logger.rhs_file_enabled = logger.rhs_file.is_open();
}

void Logger::set_verbosity(Level level) {
    verbosity = level;
}

} // namespace eulercpp
