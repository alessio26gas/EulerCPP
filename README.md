# EulerCPP

![C++](https://img.shields.io/badge/language-C++-00599C.svg) 
![CMake](https://img.shields.io/badge/build-CMake-brightgreen.svg) 
![OpenMP](https://img.shields.io/badge/parallel-OpenMP-orange.svg) 
![Doxygen](https://img.shields.io/badge/docs-Doxygen-blue.svg) 

**EulerCPP** is a standalone CFD solver for the **compressible, inviscid Euler equations**, written in modern C++.  
It supports simulations in **1D, 2D (planar or axisymmetric), and full 3D geometries** on general **polyhedral meshes**.  

At its core, EulerCPP uses the **HLLC Riemann solver** for flux computation, combined with **MUSCL reconstruction** and slope limiters to achieve second-order spatial accuracy. Time integration is explicit and multi-stage, suitable for unsteady problems and shock-dominated flows.  

Results are exported in the standard **VTK format** (ASCII or binary) or **CSV format**, making them easy to visualize with ParaView or similar post-processing tools. Parallelism is supported through **OpenMP**, enabling efficient use of multi-core CPUs.  

## Installation

### Requirements

- [CMake](https://cmake.org/) (≥ 3.10)
- C++17 or newer compiler
- [OpenMP](https://www.openmp.org) (for parallelism)
- [Ninja](https://ninja-build.org) (for Windows)

### Building

Use helper scripts:
```bash
# MacOS / Linux
./build.sh

# Windows (Powershell)
.\build.ps1
```

or build manually:
```bash
# MacOS / Linux
mkdir build
cd build
cmake ..
make

# Windows
mkdir build
cd build
cmake -G "Ninja" ..
cmake --build .
```
This will create the `eulercpp` executable in the `bin/` directory.

## Usage

Run a simulation by providing an input file:

    ./eulercpp input.inp

### Input file format
```
# LOGGER
verbosity=4
log_file=output.log

# PHYSICS
dimension=2

# MESH
mesh_file=mesh.msh
min_volume=1e-20

# FLUID
R=287.0
gamma=1.4

# NUMERICAL
time_stages=5
a=0.25,0.1666667,0.375,0.5,1.0
CFL=0.8
maxtime=10.0
maxiter=100000
reconstruction=1
limiter=3

# OUTPUT
output_format=0
output_delay=1000
prints_delay=10
prints_info_delay=200
restart_delay=1000
output_folder=output
output_name=output

# INITIAL CONDITIONS
restart=0
restart_file=output/output.restart
initial_variables=0
additional_blocks=1

# Default
T_0=300.0
p_0=101325.0
u_0=3471.8
v_0=0.0
w_0=0.0

# Additional block 1
T_1=300.0
p_1=101325.0
u_0=1735.9
v_0=0.0
w_0=0.0

# BOUNDARY CONDITIONS
n_boundaries=4
bc_1=0
bc_1_var_1=10.0
bc_1_var_2=101325.0
bc_1_var_3=300.0
bc_1_var_4=0.0
bc_1_var_5=0.0

bc_2=6
bc_2_ymax=0.01
bc_2_xmin=-0.0261
bc_2_xmax=-0.0093

bc_3=1
bc_3_xmin=0.0185

bc_4=9
bc_4_ymax=0.0
```

## API Reference
Full API reference generated with [Doxygen](https://www.doxygen.nl), with the help of ChatGPT.

See the provided Doxyfile and build with:

    doxygen Doxyfile

## Changelog

See [CHANGELOG.md](CHANGELOG.md)

## License

See [LICENSE](LICENSE)
