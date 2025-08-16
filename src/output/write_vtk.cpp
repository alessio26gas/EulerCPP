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
 * @file write_vtk.cpp
 * @brief Implementation of functions to save simulation data in VTK format.
 *
 * Provides both ASCII and binary VTK writers for EulerCPP simulations.
 *
 * Each function takes a Simulation object and a file path and writes
 * the mesh and field data in the appropriate format for visualization.
 *
 * @author Alessio Improta
 */

#include <fstream>
#include <cmath>
#include <iomanip>

#include <eulercpp/mesh/elements.hpp>
#include <eulercpp/simulation/simulation.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp {

void write_vtk_ascii(const Simulation& sim, const std::string& filepath) {
    Logger::info() << "Saving solution as VTK ASCII...";

    const Mesh& mesh = sim.mesh;
    const Fields& fields = sim.fields;
    const Input& input = sim.input;

    std::ofstream ofs(filepath + ".vtk");
    if (!ofs) {
        Logger::warning() << "Failed to open file: " << filepath << ".vtk";
        return;
    }

    ofs << std::scientific << std::setprecision(7);

    ofs << "# vtk DataFile Version 3.0\n";
    ofs << "CFD Solution\n";
    ofs << "ASCII\n";
    ofs << "DATASET UNSTRUCTURED_GRID\n";

    ofs << "POINTS " << mesh.n_nodes << " float\n";
    for (const auto& node : mesh.nodes) {
        ofs << node.position[0] << " "
            << node.position[1] << " "
            << node.position[2] << "\n";
    }

    int total_indices = 0;
    for (const auto& elem : mesh.elements) {
        if (elem.type == ElementType::POLYHEDRON) {
            int cell_size = 1;
            int pos = 0;
            for (int f = 0; f < elem.n_faces; ++f) {
                int face_nodes = elem.nodes[pos++];
                cell_size += 1 + face_nodes;
                pos += face_nodes;
            }
            total_indices += 1 + cell_size;
        } else {
            total_indices += elem.n_nodes + 1;
        }
    }

    ofs << "CELLS " << mesh.n_elements << " " << total_indices << "\n";
    for (const auto& elem : mesh.elements) {
        if (elem.type == ElementType::POLYHEDRON) {
            int pos = 0;
            int cell_size = 1;
            for (int f = 0; f < elem.n_faces; ++f) {
                int face_nodes = elem.nodes[pos++];
                cell_size += 1 + face_nodes;
                pos += face_nodes;
            }

            ofs << cell_size << " " << elem.n_faces << " ";
            pos = 0;
            for (int f = 0; f < elem.n_faces; ++f) {
                int face_nodes = elem.nodes[pos++];
                ofs << face_nodes << " ";
                for (int fn = 0; fn < face_nodes; ++fn) {
                    ofs << elem.nodes[pos++] << " ";
                }
            }
            ofs << "\n";
        } else {
            ofs << elem.n_nodes << " ";
            for (int j = 0; j < elem.n_nodes; ++j) {
                ofs << elem.nodes[j] << " ";
            }
            ofs << "\n";
        }
    }

    ofs << "CELL_TYPES " << mesh.n_elements << "\n";
    for (const auto& elem : mesh.elements) {
        int vtk_type = 0;
        switch (elem.type) {
            case ElementType::LINEAR:        vtk_type = 3; break;
            case ElementType::TRIA:          vtk_type = 5; break;
            case ElementType::QUAD:          vtk_type = 9; break;
            case ElementType::POLYGON:       vtk_type = 7; break;
            case ElementType::TETRA:         vtk_type = 10; break;
            case ElementType::HEXA:          vtk_type = 12; break;
            case ElementType::PRISM:         vtk_type = 13; break;
            case ElementType::PYRAMID:       vtk_type = 14; break;
            case ElementType::POLYHEDRON:    vtk_type = 42; break;
            default:
                Logger::warning() << "Unknown element type.";
                vtk_type = 42;
        }
        ofs << vtk_type << "\n";
    }

    ofs << "CELL_DATA " << mesh.n_elements << "\n";

    ofs << "SCALARS Density float 1\n";
    ofs << "LOOKUP_TABLE default\n";
    for (int i = 0; i < mesh.n_elements; ++i) {
        ofs << fields.W(i, 0) << "\n";
    }

    ofs << "VECTORS Momentum float\n";
    for (int i = 0; i < mesh.n_elements; ++i) {
        ofs << fields.W(i, 1) << " "
            << fields.W(i, 2) << " "
            << fields.W(i, 3) << "\n";
    }

    ofs << "SCALARS Energy float 1\n";
    ofs << "LOOKUP_TABLE default\n";
    for (int i = 0; i < mesh.n_elements; ++i) {
        ofs << fields.W(i, 4) << "\n";
    }

    ofs.close();
}

inline int to_big_endian(int value) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    unsigned char* p = reinterpret_cast<unsigned char*>(&value);
    std::reverse(p, p + sizeof(int));
#endif
    return value;
}

inline float to_big_endian(float value) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    unsigned char* p = reinterpret_cast<unsigned char*>(&value);
    std::reverse(p, p + sizeof(float));
#endif
    return value;
}

void write_vtk_bin(const Simulation& sim, const std::string& filepath) {
    Logger::info() << "Saving solution as VTK binary...";

    const Input& input = sim.input;
    const Mesh& mesh = sim.mesh;
    const Fields& fields = sim.fields;

    std::ofstream ofs(filepath + ".vtk", std::ios::binary);
    if (!ofs) {
        Logger::warning() << "Failed to open file: " << filepath << ".vtk";
        return;
    }

    // Header
    ofs << "# vtk DataFile Version 3.0\n";
    ofs << "CFD Solution\n";
    ofs << "BINARY\n";
    ofs << "DATASET UNSTRUCTURED_GRID\n";

    // Points
    ofs << "POINTS " << mesh.n_nodes << " float\n";
    for (const auto& node : mesh.nodes) {
        float x = to_big_endian(static_cast<float>(node.position[0]));
        float y = to_big_endian(static_cast<float>(node.position[1]));
        float z = to_big_endian(static_cast<float>(node.position[2]));
        ofs.write(reinterpret_cast<char*>(&x), sizeof(float));
        ofs.write(reinterpret_cast<char*>(&y), sizeof(float));
        ofs.write(reinterpret_cast<char*>(&z), sizeof(float));
    }
    ofs << "\n";

    // Cell connectivity
    int total_indices = 0;
    for (const auto& elem : mesh.elements) {
        if (elem.type == ElementType::POLYHEDRON) {
            int cell_size = 1;
            int pos = 0;
            for (int f = 0; f < elem.n_faces; ++f) {
                int face_nodes = elem.nodes[pos++];
                cell_size += 1 + face_nodes;
                pos += face_nodes;
            }
            total_indices += 1 + cell_size;
        } else {
            total_indices += elem.n_nodes + 1;
        }
    }

    // Connectivity
    ofs << "CELLS " << mesh.n_elements << " " << total_indices << "\n";
    for (const auto& elem : mesh.elements) {
        if (elem.type == ElementType::POLYHEDRON) {
            int pos = 0;
            int cell_size = 1; // number_of_faces
            for (int f = 0; f < elem.n_faces; ++f) {
                int face_nodes = elem.nodes[pos++];
                cell_size += 1 + face_nodes;
                pos += face_nodes;
            }

            int be_size  = to_big_endian(cell_size);
            int be_faces = to_big_endian(elem.n_faces);
            ofs.write(reinterpret_cast<char*>(&be_size), sizeof(int));
            ofs.write(reinterpret_cast<char*>(&be_faces), sizeof(int));

            pos = 0;
            for (int f = 0; f < elem.n_faces; ++f) {
                int face_nodes = elem.nodes[pos++];
                int be_face_nodes = to_big_endian(face_nodes);
                ofs.write(
                    reinterpret_cast<char*>(&be_face_nodes), sizeof(int)
                );

                for (int fn = 0; fn < face_nodes; ++fn) {
                    int node_id = elem.nodes[pos++];
                    int be_node_id = to_big_endian(node_id);
                    ofs.write(
                        reinterpret_cast<char*>(&be_node_id), sizeof(int)
                    );
                }
            }
        } else {
            int be_n_nodes = to_big_endian(elem.n_nodes);
            ofs.write(reinterpret_cast<char*>(&be_n_nodes), sizeof(int));
            for (int j = 0; j < elem.n_nodes; ++j) {
                int node_id = elem.nodes[j];
                int be_node_id = to_big_endian(node_id);
                ofs.write(reinterpret_cast<char*>(&be_node_id), sizeof(int));
            }
        }
    }
    ofs << "\n";

    // Cell types
    ofs << "CELL_TYPES " << mesh.n_elements << "\n";
    for (const auto& elem : mesh.elements) {
        int vtk_type = 0;
        switch (elem.type) {
            case ElementType::LINEAR:        vtk_type = 3;  break;
            case ElementType::TRIA:          vtk_type = 5;  break;
            case ElementType::QUAD:          vtk_type = 9;  break;
            case ElementType::POLYGON:       vtk_type = 7;  break;
            case ElementType::TETRA:         vtk_type = 10; break;
            case ElementType::HEXA:          vtk_type = 12; break;
            case ElementType::PRISM:         vtk_type = 13; break;
            case ElementType::PYRAMID:       vtk_type = 14; break;
            case ElementType::POLYHEDRON:    vtk_type = 42; break;
            default:            vtk_type = 42; break;
        }
        vtk_type = to_big_endian(vtk_type);
        ofs.write(reinterpret_cast<char*>(&vtk_type), sizeof(int));
    }
    ofs << "\n";


    // Cell data
    ofs << "CELL_DATA " << mesh.n_elements << "\n";

    auto write_scalar = [&](const char* name, auto accessor) {
        ofs << "SCALARS " << name << " float 1\n";
        ofs << "LOOKUP_TABLE default\n";
        for (int i = 0; i < mesh.n_elements; ++i) {
            float val = to_big_endian(static_cast<float>(accessor(i)));
            ofs.write(reinterpret_cast<char*>(&val), sizeof(float));
        }
        ofs << "\n";
    };

    write_scalar("Density", [&](int i) { return fields.W(i, 0); });
    write_scalar("Energy", [&](int i) { return fields.W(i, 4); });

    // Velocity vector
    ofs << "VECTORS Momentum float\n";
    for (int i = 0; i < mesh.n_elements; ++i) {
        float vx = to_big_endian(static_cast<float>(fields.W(i, 1)));
        float vy = to_big_endian(static_cast<float>(fields.W(i, 2)));
        float vz = to_big_endian(static_cast<float>(fields.W(i, 3)));
        ofs.write(reinterpret_cast<char*>(&vx), sizeof(float));
        ofs.write(reinterpret_cast<char*>(&vy), sizeof(float));
        ofs.write(reinterpret_cast<char*>(&vz), sizeof(float));
    }
    ofs << "\n";

    ofs.close();
}

} // namespace eulercpp
