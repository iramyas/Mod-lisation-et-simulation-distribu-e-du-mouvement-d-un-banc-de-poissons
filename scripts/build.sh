#!/bin/bash

# ============================================================================
# build.sh - Compile le projet avec Kokkos et MPI
# 
# Utilisation :
#   ./scripts/build.sh              # Build en Release
#   ./scripts/build.sh Debug        # Build en Debug
#   ./scripts/build.sh Clean        # Clean et rebuild
#
# ============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

BUILD_TYPE="${1:-Release}"

echo "##############################################################"
echo "#           Building Flocking Simulation Project             #"
echo "##############################################################"
echo ""
echo "PROJECT ROOT: $PROJECT_ROOT"
echo "BUILD DIR:    $BUILD_DIR"
echo "BUILD TYPE:   $BUILD_TYPE"
echo ""

# Clean if requested
if [ "$1" == "Clean" ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    BUILD_TYPE="Release"
fi

# Create build directory
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# Build
echo "Building project with $BUILD_TYPE build..."
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DENABLE_KOKKOS=ON \
    -DUSE_SFML=ON
cmake --build . -j$(nproc) --config "$BUILD_TYPE"

echo ""
echo "Build finished successfully !"
echo ""
echo "Executables in: $BUILD_DIR/bin/"
echo ""
echo "Available targets:"
echo "  - ./bin/main_simulation.bin       Main simulation"
echo "  - ./bin/test_mpi.bin             MPI unit tests (mpirun -n 4)"
echo "  - ./bin/test_coherence.bin       Coherence tests (mpirun -n 4)"
echo "  - ./bin/test_flock.bin           Flock unit tests"
echo "  - ./bin/benchmark                Sequential benchmark"
echo "  - ./bin/benchmark_mpi            Distributed benchmark"
echo ""

