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
 * @file shapes.cpp
 * @brief Implements geometric property computations for triangles,
 *        tetrahedra, polygons, and polyhedra.
 *
 * This file contains the implementation of shape-related functions,
 * including centroid, surface vector, volume, and characteristic length
 * calculations. These geometric primitives are used in mesh processing,
 * finite volume integration, and gradient reconstruction.
 *
 * Functions handle both simple (triangle, tetrahedron) and complex
 * (polygon, prism, hexahedron, pyramid, general polyhedron) geometries.
 *
 * @author Alessio Improta
 */

#include <cmath>

#include <eulercpp/math/vectors.hpp>
#include <eulercpp/mesh/mesh.hpp>
#include <eulercpp/mesh/shapes.hpp>

namespace eulercpp {

/**
 * @brief Helper function for polyhedron decomposition.
 *
 * Adds a tetrahedron contribution to the accumulated polyhedron
 * centroid and volume. The tetrahedron is defined by four vertices.
 *
 * @param a First vertex of the tetrahedron.
 * @param b Second vertex of the tetrahedron.
 * @param c Third vertex of the tetrahedron.
 * @param d Fourth vertex of the tetrahedron.
 * @param total_volume Accumulator for the total volume of the polyhedron.
 * @param centroid_sum Accumulator for the weighted centroid sum.
 */
static void add_tetra(const Point3D& a,
                      const Point3D& b,
                      const Point3D& c,
                      const Point3D& d,
                      double& total_volume,
                      Point3D& centroid_sum) {
    double vol = tetra_volume(a, b, c, d);
    Point3D tet_centroid = {
        (a[0] + b[0] + c[0] + d[0]) / 4.0,
        (a[1] + b[1] + c[1] + d[1]) / 4.0,
        (a[2] + b[2] + c[2] + d[2]) / 4.0
    };

    for (int i = 0; i < 3; ++i)
        centroid_sum[i] += vol * tet_centroid[i];

    total_volume += vol;
}

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
                      const Point3D& p3) {
    return {
        (p1[0] + p2[0] + p3[0]) / 3.0,
        (p1[1] + p2[1] + p3[1]) / 3.0,
        (p1[2] + p2[2] + p3[2]) / 3.0
    };
}

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
                    const Point3D& p3) {
    Point3D v1 = {p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]};
    Point3D v2 = {p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2]};
    Point3D res = math::cross_product(v1, v2);
    for (double& val : res) val *= 0.5;
    return res;
}

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
                       const Point3D& p4) {
    return {
        (p1[0] + p2[0] + p3[0] + p4[0]) / 4.0,
        (p1[1] + p2[1] + p3[1] + p4[1]) / 4.0,
        (p1[2] + p2[2] + p3[2] + p4[2]) / 4.0
    };
}

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
                    const Point3D& p4) {
    Point3D AB = {p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]};
    Point3D AC = {p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2]};
    Point3D AD = {p4[0] - p1[0], p4[1] - p1[1], p4[2] - p1[2]};
    Point3D cross = math::cross_product(AC, AD);
    double dot = math::dot_product(AB, cross);
    return std::abs(dot) / 6.0;
}

/**
 * @brief Computes centroid, area, and characteristic length of a polygon.
 *
 * @param nodes List of polygon vertices in order.
 * @return Tuple (centroid, area, characteristic length).
 */
std::tuple<Point3D, double, double>
polygon_properties(const std::vector<Point3D>& nodes) {
    const int n = nodes.size();

    Point3D h = {0.0, 0.0, 0.0};
    for (const auto& p : nodes) {
        h[0] += p[0];
        h[1] += p[1];
        h[2] += p[2];
    }
    h[0] /= n; h[1] /= n; h[2] /= n;

    Point3D Stot = {0.0, 0.0, 0.0};
    Point3D centroid = {0.0, 0.0, 0.0};
    double total_area = 0.0;

    double min_edge_length = std::numeric_limits<double>::max();

    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;

        const auto& p1 = nodes[i];
        const auto& p2 = nodes[j];

        double dist = math::distance(p1, p2);
        min_edge_length = std::min(min_edge_length, dist);

        Point3D S = tria_vector(h, p1, p2);

        Stot[0] += S[0];
        Stot[1] += S[1];
        Stot[2] += S[2];

        double area = math::norm(S);
        total_area += area;

        Point3D tri_centroid = tria_centroid(h, p1, p2);

        centroid[0] += area * tri_centroid[0];
        centroid[1] += area * tri_centroid[1];
        centroid[2] += area * tri_centroid[2];
    }

    centroid[0] /= total_area;
    centroid[1] /= total_area;
    centroid[2] /= total_area;

    double area = math::norm(Stot);
    double l = std::min(min_edge_length, std::sqrt(area));

    return {centroid, area, l};
}

/**
 * @brief Computes centroid, volume, and characteristic length of a hexahedron.
 *
 * Assumes the hexahedron is convex and nodes are ordered consistently.
 *
 * @param nodes List of 8 hexahedron vertices.
 * @return Tuple (centroid, volume, characteristic length).
 */
std::tuple<Point3D, double, double>
hexa_properties(const std::vector<Point3D>& nodes) {
    static const int hexa_index[24] = {
        1, 2, 3, 4,
        1, 5, 6, 2,
        1, 4, 8, 5,
        2, 6, 7, 3,
        3, 7, 8, 4,
        5, 8, 7, 6
    };

    Point3D H = {0.0, 0.0, 0.0};
    for (const auto& p : nodes)
        for (int i = 0; i < 3; ++i)
            H[i] += p[i];
    for (int i = 0; i < 3; ++i)
        H[i] /= 8.0;

    double total_volume = 0.0;
    Point3D centroid = {0.0, 0.0, 0.0};

    for (int f = 0; f < 6; ++f) {
        Point3D h = {0.0, 0.0, 0.0};
        std::array<int, 4> p;
        for (int i = 0; i < 4; ++i) {
            p[i] = hexa_index[f * 4 + i] - 1;
            for (int d = 0; d < 3; ++d)
                h[d] += nodes[p[i]][d];
        }
        for (int d = 0; d < 3; ++d)
            h[d] /= 4.0;

        for (int i = 0; i < 4; ++i) {
            int j = (i + 1) % 4;
            add_tetra(H, h, nodes[p[i]], nodes[p[j]], total_volume, centroid);
        }
    }

    for (int i = 0; i < 3; ++i)
        centroid[i] /= total_volume;

    double l = std::cbrt(total_volume);

    return {centroid, total_volume, l};
}

/**
 * @brief Computes centroid, volume, and characteristic length of a prism.
 *
 * @param nodes List of prism vertices.
 * @return Tuple (centroid, volume, characteristic length).
 */
std::tuple<Point3D, double, double>
prism_properties(const std::vector<Point3D>& nodes) {
    static const int prism_index[18] = {
        1, 4, 6, 3,
        2, 3, 6, 5,
        1, 2, 5, 4,
        3, 2, 1,
        4, 5, 6
    };

    Point3D H = {0.0, 0.0, 0.0};
    for (const auto& p : nodes)
        for (int i = 0; i < 3; ++i)
            H[i] += p[i];
    for (int i = 0; i < 3; ++i)
        H[i] /= 6.0;

    double total_volume = 0.0;
    Point3D centroid = {0.0, 0.0, 0.0};

    for (int f = 0; f < 3; ++f) {
        std::array<int, 4> p;
        for (int i = 0; i < 4; ++i)
            p[i] = prism_index[f * 4 + i] - 1;

        Point3D h = {0.0, 0.0, 0.0};
        for (int i = 0; i < 4; ++i)
            for (int d = 0; d < 3; ++d)
                h[d] += nodes[p[i]][d];
        for (int d = 0; d < 3; ++d)
            h[d] /= 4.0;

        for (int i = 0; i < 4; ++i) {
            int j = (i + 1) % 4;
            add_tetra(H, h, nodes[p[i]], nodes[p[j]], total_volume, centroid);
        }
    }

    for (int f = 3; f < 5; ++f) {
        std::array<int, 3> p;
        for (int i = 0; i < 3; ++i)
            p[i] = prism_index[12 + (f - 3) * 3 + i] - 1;

        add_tetra(
            H, nodes[p[0]], nodes[p[1]], nodes[p[2]], total_volume, centroid
        );
    }

    for (int i = 0; i < 3; ++i)
        centroid[i] /= total_volume;

    double l = std::cbrt(total_volume);

    return {centroid, total_volume, l};
}

/**
 * @brief Computes centroid, volume, and characteristic length of a pyramid.
 *
 * @param nodes List of pyramid vertices.
 * @return Tuple (centroid, volume, characteristic length).
 */
std::tuple<Point3D, double, double>
pyramid_properties(const std::vector<Point3D>& nodes) {
    static const int pyramid_index[16] = {
        4, 3, 2, 1,
        1, 2, 5,
        2, 3, 5,
        3, 4, 5,
        4, 1, 5
    };

    Point3D H = {0.0, 0.0, 0.0};
    for (const auto& p : nodes)
        for (int i = 0; i < 3; ++i)
            H[i] += p[i];
    for (int i = 0; i < 3; ++i)
        H[i] /= 5.0;

    double total_volume = 0.0;
    Point3D centroid = {0.0, 0.0, 0.0};

    std::array<int, 4> p;
    for (int i = 0; i < 4; ++i)
        p[i] = pyramid_index[i] - 1;

    Point3D h = {0.0, 0.0, 0.0};
    for (int i = 0; i < 4; ++i)
        for (int d = 0; d < 3; ++d)
            h[d] += nodes[p[i]][d];
    for (int d = 0; d < 3; ++d)
        h[d] /= 4.0;

    for (int i = 0; i < 4; ++i) {
        int j = (i + 1) % 4;
        add_tetra(H, h, nodes[p[i]], nodes[p[j]], total_volume, centroid);
    }

    for (int f = 1; f < 5; ++f) {
        std::array<int, 3> p;
        for (int i = 0; i < 3; ++i)
            p[i] = pyramid_index[4 + (f - 1) * 3 + i] - 1;

        add_tetra(
            H, nodes[p[0]], nodes[p[1]], nodes[p[2]], total_volume, centroid
        );
    }

    for (int i = 0; i < 3; ++i)
        centroid[i] /= total_volume;

    double l = std::cbrt(total_volume);

    return {centroid, total_volume, l};
}

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
                      Mesh& mesh) {
    Point3D H = {0.0, 0.0, 0.0};
    int total_vertices = 0;
    std::vector<int> all_vertex_indices;

    size_t index = 0;
    for (int f = 0; f < n_faces; ++f) {
        int N = nodes[index++];
        total_vertices += N;
        for (int i = 0; i < N; ++i)
            all_vertex_indices.push_back(nodes[index++]);
    }

    for (int idx : all_vertex_indices)
        for (int d = 0; d < 3; ++d)
            H[d] += mesh.nodes[idx].position[d];
    for (int d = 0; d < 3; ++d)
        H[d] /= static_cast<double>(total_vertices);

    double total_volume = 0.0;
    Point3D centroid = {0.0, 0.0, 0.0};

    index = 0;
    for (int f = 0; f < n_faces; ++f) {
        int N = nodes[index++];

        std::vector<int> face_indices(
            nodes.begin() + index, nodes.begin() + index + N
        );
        index += N;

        Point3D h = {0.0, 0.0, 0.0};
        for (int vi : face_indices)
            for (int d = 0; d < 3; ++d)
                h[d] += mesh.nodes[vi].position[d];
        for (int d = 0; d < 3; ++d)
            h[d] /= static_cast<double>(N);

        for (int i = 0; i < N; ++i) {
            int j = (i + 1) % N;
            const Point3D& a = mesh.nodes[face_indices[i]].position;
            const Point3D& b = mesh.nodes[face_indices[j]].position;

            add_tetra(H, h, a, b, total_volume, centroid);
        }
    }

    for (int i = 0; i < 3; ++i)
        centroid[i] /= total_volume;

    double l = std::cbrt(total_volume);

    return {centroid, total_volume, l};
}

} // namespace eulercpp
