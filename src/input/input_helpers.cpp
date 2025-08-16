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
    @file input_helpers.cpp
    @brief Utility functions for parsing and processing simulation input data.

    This file provides helper routines for reading configuration files and
    converting raw string data into structured formats. It is primarily
    used by the input system to:
      - Parse configuration key-value pairs from text files.
      - Split and convert comma-separated values to numeric vectors.
      - Trim and sanitize string data before interpretation.

    Functions include:
      - parse_config_file(): Reads and parses configuration files into a
        key-value map.
      - parse_vector(): Converts a comma-separated string into doubles.
      - parse_int_vector(): Converts a comma-separated string into integers.

    These helpers are intentionally minimal and independent to maintain
    reusability across different parts of the codebase.

    @see parse_config_file, parse_vector, parse_int_vector
    @author Alessio Improta
    @details
      All parsing functions ignore malformed entries gracefully by applying
      default values (0.0 for doubles, 0 for integers).
 */

#include <array>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

#include <eulercpp/output/logger.hpp>

namespace eulercpp {

/**
 * @brief Removes leading and trailing whitespace from a string.
 *
 * Whitespace characters include spaces, tabs, carriage returns,
 * and newline characters.
 *
 * @param s String to be trimmed.
 * @return A new string without leading or trailing whitespace.
 */
static std::string trim(const std::string& s) {
    const char* whitespace = " \t\r\n";
    const auto begin = s.find_first_not_of(whitespace);
    if (begin == std::string::npos) return "";
    const auto end = s.find_last_not_of(whitespace);
    return s.substr(begin, end - begin + 1);
}

/**
 * @brief Parses a configuration file into key-value pairs.
 *
 * Opens the specified file and reads it line-by-line. Each non-empty
 * line containing an '=' character is split into a key and a value.
 * Both are trimmed of whitespace before being stored in the map.
 *
 * Lines that are empty or lack an '=' separator are ignored.
 *
 * @param filename Path to the configuration file.
 * @return std::map<std::string, std::string> containing parsed settings.
 * @throws std::invalid_argument if the file cannot be opened.
 *
 * @note Overwrites previous entries if a key appears multiple times.
 */
std::map<std::string, std::string> parse_config_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::invalid_argument("Cannot open input file " + filename);
    }

    std::map<std::string, std::string> config;
    std::string line;
    while (std::getline(file, line)) {
        line.erase(line.find_last_not_of("\r\n") + 1);

        if (line.empty() || line.find('=') == std::string::npos)
            continue;

        std::string key, value;
        std::istringstream iss(line);
        std::getline(iss, key, '=');
        std::getline(iss, value);

        key = trim(key);
        value = trim(value);

        config[key] = value;
    }
    return config;
}

/**
 * @brief Converts a comma-separated string into a vector of doubles.
 *
 * Each value in the string is parsed using `std::stod`. If parsing
 * fails for a value, it is replaced with `0.0`.
 *
 * @param str Comma-separated numeric string.
 * @return Vector of double values.
 */
std::vector<double> parse_vector(const std::string& str) {
    std::vector<double> result;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, ',')) {
        try {
            result.push_back(std::stod(item));
        } catch (...) {
            result.push_back(0.0);
        }
    }
    return result;
}

/**
 * @brief Converts a comma-separated string into a vector of integers.
 *
 * Each value in the string is parsed using `std::stoi`. If parsing
 * fails for a value, it is replaced with `0`.
 *
 * @param str Comma-separated numeric string.
 * @return Vector of integer values.
 */
std::vector<int> parse_int_vector(const std::string& str) {
    std::vector<int> result;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, ',')) {
        try {
            result.push_back(std::stoi(item));
        } catch (...) {
            result.push_back(0);
        }
    }
    return result;
}

} // namespace eulercpp
