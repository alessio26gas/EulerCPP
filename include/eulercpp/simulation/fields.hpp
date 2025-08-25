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
 * @file fields.hpp
 * @brief Defines the Fields class holding all simulation variables and fluxes.
 *
 * This header provides the Fields class, which stores the conservative 
 * variables, source terms, fluxes, gradients, and RHS vectors for a 
 * simulation. Fields are stored in a contiguous, cache-friendly layout 
 * for efficient access.
 *
 * @author Alessio Improta
 */

#pragma once

#include <vector>
#include <array>
#include <cstring>
#include <omp.h>

#include <eulercpp/mesh/mesh.hpp>
#include <eulercpp/output/logger.hpp>

namespace eulercpp {

/**
 * @class Fields
 * @brief Manages all field data for the simulation.
 *
 * The Fields class contains:
 * - Conservative variables (current and previous iteration)
 * - Source terms
 * - Gradients of conservative variables
 * - Face-centered values and fluxes
 * - RHS vectors for residual computations
 *
 * Provides accessors for cell-based and face-based data, and utility 
 * functions for initialization and solution updates.
 */
class Fields {
public:
    /**
     * @brief Default constructor.
     *
     * Initializes an empty Fields object. Call init() before using.
     */
    Fields() = default;

    /**
     * @brief Returns the spatial dimension of the simulation.
     * @return Dimension (1, 2 or 3)
     */
    inline const int& dimension() const noexcept {
        return dim;
    }

    /**
     * @brief Access the conservative variable W for a given cell 
     * and variable index.
     * @param cell Index of the cell
     * @param var Index of the conservative variable
     * @return Reference to the conservative variable W[cell, var]
     */
    inline double& W(int cell, int var) noexcept {
        return conservatives[cell * n_var + var];
    }

    /**
     * @brief Const access to the conservative variable W for a given 
     * cell and variable index.
     * @param cell Index of the cell
     * @param var Index of the conservative variable
     * @return Const reference to the conservative variable W[cell, var]
     */
    inline const double& W(int cell, int var) const noexcept {
        return conservatives[cell * n_var + var];
    }

    /**
     * @brief Provides direct pointer access to the underlying array 
     * of conservative variables.
     * 
     * Allows for high-performance bulk operations such as reading/writing
     * binary data without using W(i,v) in loops.
     * 
     * @return Pointer to the first element of the conservatives array
     */
    inline double* Wdata() noexcept {
        return conservatives.data();
    }

    /**
     * @brief Provides const direct pointer access to the underlying 
     * array of conservative variables.
     * 
     * Allows for high-performance bulk read operations on the array.
     * 
     * @return Const pointer to the first element of the conservatives array
     */
    inline const double* Wdata() const noexcept {
        return conservatives.data();
    }

    /**
     * @brief Access the conservative variable W from the previous iteration.
     * @param cell Index of the cell
     * @param var Index of the conservative variable
     * @return Reference to the old conservative variable Wold[cell, var]
     */
    inline double& Wold(int cell, int var) noexcept {
        return conservatives_old[cell * n_var + var];
    }

    /**
     * @brief Const access to the previous iteration conservative variable.
     * @param cell Index of the cell
     * @param var Index of the conservative variable
     * @return Const reference to the old conservative variable Wold[cell, var]
     */
    inline const double& Wold(int cell, int var) const noexcept {
        return conservatives_old[cell * n_var + var];
    }

    /**
     * @brief Access the source term S for a given cell and variable.
     * @param cell Index of the cell
     * @param var Index of the variable
     * @return Reference to the source term S[cell, var]
     */
    inline double& S(int cell, int var) noexcept {
        return sources[cell * n_var + var];
    }

    /**
     * @brief Const access to the source term S.
     * @param cell Index of the cell
     * @param var Index of the variable
     * @return Const reference to the source term S[cell, var]
     */
    inline const double& S(int cell, int var) const noexcept {
        return sources[cell * n_var + var];
    }

    /**
     * @brief Access the gradient of the conservative variable W 
     * at a given cell and variable.
     * @param cell Index of the cell
     * @param var Index of the variable
     * @return Reference to the 3D gradient gradW[cell, var]
     */
    inline std::array<double, 3>&
    gradW(int cell, int var) noexcept {
        return grad_conservatives[cell * n_var + var];
    }

    /**
     * @brief Const access to the gradient of the conservative variable W.
     * @param cell Index of the cell
     * @param var Index of the variable
     * @return Const reference to the 3D gradient gradW[cell, var]
     */
    inline const std::array<double, 3>&
    gradW(int cell, int var) const noexcept {
        return grad_conservatives[cell * n_var + var];
    }

    /**
     * @brief Access the face-reconstructed conservative variable Wf.
     * @param face Index of the face
     * @param var Index of the variable
     * @return Reference to Wface[face, var]
     */
    inline double& Wf(int face, int var) noexcept {
        return Wface[face * n_var + var];
    }

    /**
     * @brief Const ccess the face-reconstructed conservative variable Wf.
     * @param face Index of the face
     * @param var Index of the variable
     * @return Const reference to Wface[face, var]
     */
    inline const double& Wf(int face, int var) const noexcept {
        return Wface[face * n_var + var];
    }

    /**
     * @brief Access the convective flux F at a given face and variable.
     * @param face Index of the face
     * @param var Index of the variable
     * @return Reference to fluxF[face, var]
     */
    inline double& F(int face, int var) noexcept {
        return fluxF[face * n_var + var];
    }

    /**
     * @brief Const access the convective flux F at a given face and variable.
     * @param face Index of the face
     * @param var Index of the variable
     * @return Const reference to fluxF[face, var]
     */
    inline const double& F(int face, int var) const noexcept {
        return fluxF[face * n_var + var];
    }

    /**
     * @brief Access the right-hand side (RHS) vector b.
     * @param cell Index of the cell
     * @param var Index of the variable
     * @return Reference to rhs[cell, var]
     */
    inline double& b(int cell, int var) noexcept {
        return rhs[cell * n_var + var];
    }

    /**
     * @brief Const access the right-hand side (RHS) vector b.
     * @param cell Index of the cell
     * @param var Index of the variable
     * @return Const reference to rhs[cell, var]
     */
    inline const double& b(int cell, int var) const noexcept {
        return rhs[cell * n_var + var];
    }

    /**
     * @brief Compute the L1 residuals over all elements.
     *
     * @return Sums of absolute RHS entries.
     */
    const std::array<double, 5> get_residuals() const noexcept {
        std::array<double, 5> residual = {0.0};
        #pragma omp parallel for
        for (int i = 0; i < n_elements; ++i) {
            for (int v = 0; v < n_var; ++v) {
                residual[v] += std::abs(rhs[i * n_var + v]);
            }
        }
        return residual;
    }

    /**
     * @brief Allocate and initialize all field arrays.
     *
     * Initializes conservative variables, source terms, gradients,
     * face values, fluxes, and RHS vectors to zero.
     *
     * @param mesh Simulation mesh
     * @param input Simulation input parameters
     */
    void init(const Mesh& mesh, const Input& input) {
        n_elements = mesh.n_elements;
        n_faces = mesh.n_faces;
        n_var = 5;

        switch(input.physics.dimension) {
            case 3:     dim = 3;    break;
            case 0:     dim = 1;    break;
            default:    dim = 2;    break;
        }

        Logger::debug() << "Allocating fields...";
        conservatives.assign(n_elements * n_var, 0.0);
        conservatives_old.assign(n_elements * n_var, 0.0);
        sources.assign(n_elements * n_var, 0.0);
        grad_conservatives.assign(n_elements * n_var, {0.0, 0.0, 0.0});
        Wface.assign(n_faces * n_var, 0.0);
        fluxF.assign(n_faces * n_var, 0.0);
        rhs.assign(n_elements * n_var, 0.0);
    }

    /**
     * @brief Prepare for a solution update.
     *
     * Copies the current conservative variables into the "old" array.
     * Must be called before advancing to the next iteration.
     */
    void prepare_solution_update() {
        std::memcpy(
            conservatives_old.data(),
            conservatives.data(),
            conservatives.size() * sizeof(double)
        );
    }

private:
    int n_elements = 0; /**< Number of elements in the mesh */
    int n_faces = 0;    /**< Number of faces in the mesh */
    int n_var = 5;      /**< Number of conservative variables */
    int dim = 0;        /**< Spatial dimension */

    std::vector<double> conservatives;      /**< Conservative variables W */
    std::vector<double> conservatives_old;  /**< Previous iteration W */
    std::vector<double> sources;            /**< Source terms S */

    std::vector<std::array<double, 3>> grad_conservatives; /**< Gradients */

    std::vector<double> rhs;        /**< RHS vector b */
    std::vector<double> Wface;      /**< Face-centered variables */
    std::vector<double> fluxF;      /**< Convective fluxes F */
};

} // namespace eulercpp
