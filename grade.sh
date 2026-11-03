#!/bin/sh
# Usage: ./grade.sh [Debug|Release|ASan] [lab number | C-1 | labs | challenges]   (default: Debug, everything)
# Builds, then runs every lab in its own process and writes grade-report.json here.
CONFIG=${1:-Debug}
cd "$(dirname "$0")"
./build.sh "$CONFIG" > /dev/null || { echo "Build failed. Run ./build.sh $CONFIG to see the errors."; exit 1; }
"./bin/$CONFIG/BugHunt" --grade $2
