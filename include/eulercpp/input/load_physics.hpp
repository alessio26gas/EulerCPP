/**
 * @file load_physics.hpp
 * @brief Declares structures and functions for physics input settings.
 *
 * Defines the Physics struct, which holds all physics parameters for
 * the simulation. Declares load_physics() to populate these parameters 
 * from a configuration map into the global Input structure.
 *
 * @author Alessio Improta
 */

#pragma once

#include <map>
#include <string>

namespace eulercpp {

struct Input;

/**
 * @struct Physics
 * @brief Holds all physics-related input settings.
 */
struct Physics {
    int dimension = 0; /**< Spatial dimension. */
};

/**
 * @brief Populate physics parameters from a configuration map.
 *
 * Expected keys:
 *   - "dimension" : integer (2 or 3).
 *         Accepts numeric codes or textual equivalents:
 *             0 → 1D
 *             1 → 2D
 *             2 → Axisymmetric
 *             3 → 3D
 *
 * If a key is absent, the corresponding field is left unchanged (keeps
 * its default value). Invalid numeric strings may throw from `std::stoi`.
 *
 * @param config Map of configuration key–value pairs (strings).
 * @param input  Input structure to be updated in-place.
 */
void load_physics(
    const std::map<std::string, std::string>& config, Input& input
);

} // namespace eulercpp
