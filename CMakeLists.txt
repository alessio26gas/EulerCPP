# This file defines the CMake build system for EulerCPP, a cross-platform
# CFD solver written in modern C++17. It handles source collection,
# compiler options, platform-specific settings, and installation rules.

# The project supports Linux, macOS, and Windows, and uses OpenMP for
# parallel execution where available.

cmake_minimum_required(VERSION 3.10)

# Project definition: name, version, languages
project(eulercpp VERSION 1.0 LANGUAGES C CXX)

# C++ standard settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Avoid compiler-specific extensions

# Output directory for compiled binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")

# Interface target for public headers
add_library(eulercpp_headers INTERFACE)
target_include_directories(eulercpp_headers INTERFACE
    "${PROJECT_SOURCE_DIR}/include"
)
target_compile_features(eulercpp_headers INTERFACE cxx_std_17)

# Automatically collect all source files
file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.c"
)

# Main executable target
add_executable(eulercpp ${SOURCES})
target_link_libraries(eulercpp PRIVATE eulercpp_headers)

# Platform-specific system libraries
if(WIN32)
    target_link_libraries(eulercpp PRIVATE kernel32 user32 gdi32)
else()
    target_link_libraries(eulercpp PRIVATE m)
endif()

# Set Release build if unspecified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler optimization flags for Release builds
if(APPLE)
    # OpenMP setup for macOS (requires Homebrew install)
    include_directories("/opt/homebrew/opt/libomp/include")
    set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -march=native -fdata-sections
                               -ffunction-sections -flto=auto 
                               -fopenmp -DNDEBUG")
else()
    set(CMAKE_C_FLAGS_RELEASE "-O3 -march=native -fdata-sections 
                               -ffunction-sections -flto=auto 
                               -ffat-lto-objects -fuse-linker-plugin 
                               -fopenmp -DNDEBUG")
endif()

# Enable Link-Time Optimization (LTO)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

# Install rules: executable to bin/, headers to include/
install(TARGETS eulercpp DESTINATION bin)
install(DIRECTORY include/eulercpp DESTINATION include)