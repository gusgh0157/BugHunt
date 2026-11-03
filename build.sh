#!/bin/sh
# Usage: ./build.sh [Debug|Release|ASan]   (default: Debug)
# Builds with CMake (CMakePresets.json) and the default C++ compiler (set CXX=clang++ or CXX=g++ to choose).
# Output: bin/<config>/BugHunt
set -e
CONFIG=${1:-Debug}
case "$CONFIG" in
    Debug) PRESET=debug ;;
    Release) PRESET=release ;;
    ASan) PRESET=asan ;;
    *) echo "Unknown configuration: $CONFIG. Use Debug, Release or ASan." >&2; exit 1 ;;
esac
cd "$(dirname "$0")"
cmake --preset "$PRESET"
cmake --build --preset "$PRESET"
echo
echo "Built: $(pwd)/bin/$CONFIG/BugHunt"
