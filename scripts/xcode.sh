#!/usr/bin/env bash
# Generate (or refresh) the Xcode project from CMake and open it. Never edit the .xcodeproj.
# Optional: RESTYLE_DEBUG_HOST="/Applications/REAPER.app" scripts/xcode.sh
source "$(dirname "$0")/_common.sh"
cd "$ROOT"
git submodule update --init --recursive
EXTRA=()
[[ -n "${RESTYLE_DEBUG_HOST:-}" ]] && EXTRA+=("-DRESTYLE_DEBUG_HOST=$RESTYLE_DEBUG_HOST")
cmake --preset xcode "${EXTRA[@]}"
open "$ROOT/build-xcode/Restyle.xcodeproj"
