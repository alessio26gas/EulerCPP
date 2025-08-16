#!/bin/bash

# This script configures and builds EulerCPP using CMake. It supports
# an optional "-clean" flag to remove the previous build folder before
# compiling.

# Exit immediately if a command fails
set -e

# Function to display usage information
usage() {
  echo "Usage: $0 [-clean]"
  echo "  -clean   Cleans the build folder before compiling"
  exit 1
}

# Default options
CLEAN=false

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
  case "$1" in
    -clean)
      CLEAN=true
      ;;
    *)
      usage
      ;;
  esac
  shift
done

# Remove previous build folder if requested
if [ "$CLEAN" = true ]; then
  rm -rf build
fi

# Create build folder if it does not exist
if [ ! -d "build" ]; then
  mkdir build
fi

# Enter build folder, configure and build with CMake
cd build

cmake ..

cmake --build .

# Return to the root directory
cd ..