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
 * @file writer.cpp
 * @brief Implementation of the Writer class for saving simulation output.
 *
 * Provides methods to save simulation data and restart files, including 
 * automatic directory creation.
 *
 * @author Alessio Improta
 */

#include <filesystem>
#include <stdexcept>
#include <sstream>
#include <iomanip>

#include <eulercpp/output/writer.hpp>
#include <eulercpp/output/write_vtk.hpp>
#include <eulercpp/output/write_csv.hpp>
#include <eulercpp/output/restart.hpp>

namespace fs = std::filesystem;

namespace eulercpp {

Writer::Format Writer::format_ = Writer::Format::VTK_BIN;
std::string Writer::output_dir_ = "./output";
std::string Writer::output_name_ = "output";

void Writer::configure(
    int fmt,
    const std::string& output_dir,
    const std::string& output_name
) {
    format_ = static_cast<Format>(fmt);
    output_dir_ = output_dir;
    output_name_ = output_name;
    fs::create_directories(output_dir_);
}

void Writer::save(const Simulation& sim) {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(6) << sim.status.iteration;
    std::string iter_str = oss.str();

    std::string filepath = (
        fs::path(output_dir_)/ (output_name_ + "_" + iter_str)
    ).string();

    switch (format_) {
        case Format::VTK_BIN:
            write_vtk_bin(sim, filepath);
            break;
        case Format::VTK_ASCII:
            write_vtk_ascii(sim, filepath);
            break;
        case Format::CSV:
            write_csv(sim, filepath);
            break;
        default:
            throw std::runtime_error("Unsupported output format");
    }
}

void Writer::save_restart(const Simulation& sim) {
    std::string filepath = (
        fs::path(output_dir_) / (output_name_ + ".restart")
    ).string();
    write_restart(sim, filepath);
}

} // namespace eulercpp
