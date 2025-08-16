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
 * @file shapes.hpp
 * @brief Geometric utility functions for computing centroids, volumes,
 *        and characteristic lengths of basic shapes (triangles,
 *        tetrahedra, polygons, and polyhedra).
 *
 * This header declares shape-related routines used throughout the
 * finite-volume framework. They provide centroid, volume/surface, and
 * characteristic length computations for 2D and 3D shapes.
 *
 * Functions are used by mesh preprocessing routines to compute element
 * properties such as centroid locations, surface vectors, and volumes.
 *
 * @author Alessio Improta
 */

#pragma once

#include <array>
#include <vector>

#include <eulercpp/mesh/nodes.hpp>

namespace eulercpp {

/**
 * @typedef Point3D
 * @brief Alias for a 3D point represented as an array of three doubles.
 */
using Point3D = std::array<double, 3>;

/**
 * @brief Computes the centroid of a triangle in 3D space.
 *
 * The centroid is the arithmetic mean of the three vertices.
 *
 * @param p1 First vertex.
 * @param p2 Second vertex.
 * @param p3 Third vertex.
 * @return Centroid of the triangle.
 */
Point3D tria_centroid(const Point3D& p1,
                      const Point3D& p2,
                      const Point3D& p3);

/**
 * @brief Computes the oriented surface vector of a triangle.
 *
 * The vector is given by the cross product of two edges divided by 2.
 * Its direction is normal to the triangle plane, and its magnitude is
 * equal to the triangle’s area.
 *
 * @param p1 First vertex.
 * @param p2 Second vertex.
 * @param p3 Third vertex.
 * @return Surface vector (normal * area).
 */
Point3D tria_vector(const Point3D& p1,
                    const Point3D& p2,
                    const Point3D& p3);

/**
 * @brief Computes the centroid of a tetrahedron in 3D space.
 *
 * The centroid is the arithmetic mean of its four vertices.
 *
 * @param p1 First vertex.
 * @param p2 Second vertex.
 * @param p3 Third vertex.
 * @param p4 Fourth vertex.
 * @return Centroid of the tetrahedron.
 */
Point3D tetra_centroid(const Point3D& p1,
                       const Point3D& p2,
                       const Point3D& p3,
                       const Point3D& p4);

/**
 * @brief Computes the signed volume of a tetrahedron.
 *
 * @param p1 First vertex.
 * @param p2 Second vertex.
 * @param p3 Third vertex.
 * @param p4 Fourth vertex.
 * @return Signed volume of the tetrahedron.
 */
double tetra_volume(const Point3D& p1,
                    const Point3D& p2,
                    const Point3D& p3,
                    const Point3D& p4);

/**
 * @brief Computes centroid, area, and characteristic length of a polygon.
 *
 * @param nodes List of polygon vertices in order.
 * @return Tuple (centroid, area, characteristic length).
 */
std::tuple<Point3D, double, double>
polygon_properties(const std::vector<Point3D>& nodes);

/**
 * @brief Computes centroid, volume, and characteristic length of a hexahedron.
 *
 * Assumes the hexahedron is convex and nodes are ordered consistently.
 *
 * @param nodes List of 8 hexahedron vertices.
 * @return Tuple (centroid, volume, characteristic length).
 */
std::tuple<Point3D, double, double>
hexa_properties(const std::vector<Point3D>& nodes);

/**
 * @brief Computes centroid, volume, and characteristic length of a prism.
 *
 * @param nodes List of prism vertices.
 * @return Tuple (centroid, volume, characteristic length).
 */
std::tuple<Point3D, double, double>
prism_properties(const std::vector<Point3D>& nodes);

/**
 * @brief Computes centroid, volume, and characteristic length of a pyramid.
 *
 * @param nodes List of pyramid vertices.
 * @return Tuple (centroid, volume, characteristic length).
 */
std::tuple<Point3D, double, double>
pyramid_properties(const std::vector<Point3D>& nodes);

/**
 * @brief Computes centroid, volume, and characteristic length of a polyhedron.
 *
 * The polyhedron may have an arbitrary number of faces and vertices.
 * Properties are computed via decomposition into tetrahedra.
 *
 * @param n_faces Number of faces.
 * @param nodes Node indices defining the polyhedron.
 * @param mesh Reference to the mesh providing node coordinates.
 * @return Tuple (centroid, volume, characteristic length).
 */
std::tuple<Point3D, double, double>
polyhedron_properties(const int n_faces,
                      const std::vector<int>& nodes,
                      Mesh& mesh);

} // namespace eulercpp
