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
 * @file load_input.hpp
 * @brief Master input loader.
 *
 * This header includes all individual input loading headers. It provides 
 * a single entry point to import the entire simulation configuration, 
 * including logger, physics, fluid, mesh, numerical settings, initial 
 * conditions, boundary conditions, and output options.
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/input/load_logger.hpp>
#include <eulercpp/input/load_physics.hpp>
#include <eulercpp/input/load_fluid.hpp>
#include <eulercpp/input/load_mesh.hpp>
#include <eulercpp/input/load_numerical.hpp>
#include <eulercpp/input/load_init.hpp>
#include <eulercpp/input/load_bc.hpp>
#include <eulercpp/input/load_output.hpp>