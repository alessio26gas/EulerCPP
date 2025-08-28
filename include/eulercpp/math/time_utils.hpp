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
 * @file time_utils.hpp
 * @brief Utility functions for formatting time durations.
 *
 * This header provides helper functions to convert time durations
 * in seconds into human-readable strings with appropriate units:
 * microseconds, milliseconds, seconds, minutes, and hours.
 *
 * The formatting adapts automatically depending on the magnitude of
 * the duration, making it convenient for logging and profiling code.
 *
 * @author Alessio Improta
 */

#pragma once

#include <string>
#include <sstream>
#include <iomanip>

namespace eulercpp::math {

/**
 * @brief Formats a duration in seconds into a human-readable string.
 *
 * The function automatically selects the appropriate time unit:
 * - μs for durations < 1e-5 s
 * - ms for durations < 1e-2 s
 * - s for durations < 60 s
 * - min + s for durations < 3600 s
 * - h + min + s for durations ≥ 3600 s
 *
 * The output string includes fixed-point formatting with a reasonable
 * number of decimal places depending on the unit.
 *
 * @param seconds Duration in seconds.
 * @return A string representing the duration in a human-readable format.
 *
 * Usage example:
 * @code
 * std::cout << format_duration(0.000012); // "12 μs"
 * std::cout << format_duration(1.234);    // "1.23 s"
 * std::cout << format_duration(125.6);    // "2 min 5.6 s"
 * @endcode
 */
inline std::string format_duration(double seconds) {
    std::ostringstream out;

    if (seconds < 1.0e-5) {
        out << std::fixed << std::setprecision(0) << seconds * 1.0e6 << " μs";
    } else if (seconds < 1.0e-2) {
        out << std::fixed << std::setprecision(2) << seconds * 1.0e3 << " ms";
    } else if (seconds < 60.0) {
        out << std::fixed << std::setprecision(2) << seconds << " s";
    } else if (seconds < 3600.0) {
        int minutes = static_cast<int>(seconds) / 60;
        double rem_seconds = seconds - (minutes * 60);
        out << minutes << " min " << std::fixed << std::setprecision(1)
            << rem_seconds << " s";
    } else {
        int hours = static_cast<int>(seconds) / 3600;
        int minutes = (static_cast<int>(seconds) % 3600) / 60;
        double rem_seconds = seconds - (hours * 3600 + minutes * 60);
        out << hours << " h " << minutes << " min " << std::fixed
            << std::setprecision(0) << rem_seconds << " s";
    }

    return out.str();
}

} // namespace eulercpp::math
