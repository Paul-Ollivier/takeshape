#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Generate (or refresh) the Xcode project from CMake and open it. Never edit the .xcodeproj.
# Optional: TAKESHAPE_DEBUG_HOST="/Applications/REAPER.app" scripts/xcode.sh
source "$(dirname "$0")/_common.sh"
cd "$ROOT"
git submodule update --init --recursive
EXTRA=()
[[ -n "${TAKESHAPE_DEBUG_HOST:-}" ]] && EXTRA+=("-DTAKESHAPE_DEBUG_HOST=$TAKESHAPE_DEBUG_HOST")
cmake --preset xcode ${EXTRA[@]+"${EXTRA[@]}"}
open "$ROOT/build-xcode/Takeshape.xcodeproj"
