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
 * @file input_helpers.hpp
 * @brief Helper functions for parsing configuration strings and files.
 *
 * This header provides utility functions for converting comma-separated 
 * strings into numeric containers (`std::vector` or `std::array`), and 
 * for parsing configuration files into key-value maps.
 *
 * @author Alessio Improta
 */

#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace eulercpp {

/**
 * @brief Converts a comma-separated string into a vector of doubles.
 *
 * Each value in the string is parsed using `std::stod`. If parsing
 * fails for a value, it is replaced with `0.0`.
 *
 * @param str Comma-separated numeric string.
 * @return Vector of double values.
 */
std::vector<double> parse_vector(const std::string& str);

/**
 * @brief Converts a comma-separated string into a vector of integers.
 *
 * Each value in the string is parsed using `std::stoi`. If parsing
 * fails for a value, it is replaced with `0`.
 *
 * @param str Comma-separated numeric string.
 * @return Vector of integer values.
 */
std::vector<int> parse_int_vector(const std::string& str);

/**
 * @brief Parse a comma-separated string into a fixed-size std::array<T, N>.
 *
 * @tparam T Data type to parse
 * @tparam N Number of elements expected
 * @param s Comma-separated input string
 * @return std::array<T, N> Parsed values
 */
template<typename T, std::size_t N>
std::array<T, N> parse_array(const std::string& s) {
    std::array<T, N> arr{};
    std::stringstream ss(s);
    std::string item;
    std::size_t i = 0;
    while (std::getline(ss, item, ',') && i < N) {
        arr[i++] = static_cast<T>(std::stod(item));
    }
    return arr;
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
std::map<std::string, std::string> parse_config_file(
    const std::string& filename
);

} // namespace eulercpp
