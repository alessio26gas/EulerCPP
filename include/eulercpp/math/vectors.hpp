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
 * @file vectors.hpp
 * @brief 3D vector utilities for mathematical operations in EulerCPP.
 *
 * This header provides basic vector algebra operations used in
 * computational fluid dynamics and numerical simulations, including:
 * - Cross product
 * - Dot product
 * - Norm computation
 * - Distance between points
 * - Midpoint computation
 * - Normalization of vectors
 *
 * All operations are designed for 3D vectors represented by
 * `std::array<double, 3>`.
 *
 * @author Alessio Improta
 */

#pragma once

#include <array>

namespace eulercpp::math {

/**
 * @typedef Vector3D
 * @brief Alias for a 3D vector represented as an array of three doubles.
 */
using Vector3D = std::array<double, 3>;

/**
 * @brief Computes the cross product of two 3D vectors.
 *
 * This function calculates the vector perpendicular to both input vectors,
 * following the right-hand rule.
 *
 * @param v1 The first input 3D vector.
 * @param v2 The second input 3D vector.
 * @return The resulting 3D vector storing the cross product.
 */
Vector3D cross_product(const Vector3D& v1, const Vector3D& v2);

/**
 * @brief Computes the dot product of two 3D vectors.
 *
 * @param a The first input 3D vector.
 * @param b The second input 3D vector.
 * @return The scalar result of the dot product.
 */
double dot_product(const Vector3D& a, const Vector3D& b);

/**
 * @brief Computes the Euclidean norm (magnitude) of a 3D vector.
 *
 * @param v The input 3D vector.
 * @return The magnitude of the vector.
 */
double norm(const Vector3D& v);

/**
 * @brief Computes the distance between two 3D points.
 *
 * This is equivalent to the norm of the vector connecting the two points.
 *
 * @param p1 The first 3D point.
 * @param p2 The second 3D point.
 * @return The Euclidean distance between p1 and p2.
 */
double distance(const Vector3D& p1, const Vector3D& p2);

/**
 * @brief Computes the midpoint between two 3D points.
 *
 * @param p1 The first 3D point.
 * @param p2 The second 3D point.
 * @return A 3D vector representing the midpoint.
 */
Vector3D mid_point(const Vector3D& p1, const Vector3D& p2);

/**
 * @brief Normalizes a 3D vector in place.
 *
 * This function modifies the input vector so that its norm becomes 1.
 * If the input vector is a zero vector, the result is undefined.
 *
 * @param v The vector to normalize.
 */
void normalize(Vector3D& v);

} // namespace eulercpp::math
