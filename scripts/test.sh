#!/usr/bin/env bash
# Unit tests for the JUCE-free core.
CONFIG="${1:-${CONFIG:-debug}}"
source "$(dirname "$0")/_common.sh"
cd "$ROOT"
cmake --build --preset "$PRESET" --target restyle_tests
ctest --preset ninja-debug
