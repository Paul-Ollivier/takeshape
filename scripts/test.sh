#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Unit tests for the JUCE-free core.
CONFIG="${1:-${CONFIG:-debug}}"
source "$(dirname "$0")/_common.sh"
cd "$ROOT"
cmake --build --preset "$PRESET" --target takeshape_tests
ctest --preset "$PRESET"
