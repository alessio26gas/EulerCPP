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
 * @file all_boundaries.hpp
 * @brief Aggregated include header for all boundary condition implementations.
 *
 * This header consolidates all available boundary condition modules into a
 * single include file. It provides convenient access to every boundary
 * condition type supported by the solver without needing to include each
 * one individually.
 *
 * @note This file is intended as a convenience header and should be used
 *       when multiple boundary condition types are required in a module.
 *
 * @author Alessio Improta
 */

#pragma once

#include <eulercpp/physics/boundaries/pressure_outlet.hpp>
#include <eulercpp/physics/boundaries/stagnation_inlet.hpp>
#include <eulercpp/physics/boundaries/subsonic_inlet.hpp>
#include <eulercpp/physics/boundaries/supersonic_inlet.hpp>
#include <eulercpp/physics/boundaries/supersonic_outlet.hpp>
#include <eulercpp/physics/boundaries/symmetry.hpp>