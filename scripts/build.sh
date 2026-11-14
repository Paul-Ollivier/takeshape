#!/usr/bin/env bash
# Usage: scripts/build.sh [debug|release]   — Ninja build (fast CLI). Copies AU/VST3 into
# ~/Library/Audio/Plug-Ins automatically (RESTYLE_COPY_AFTER_BUILD=ON).
CONFIG="${1:-${CONFIG:-debug}}"
source "$(dirname "$0")/_common.sh"
cd "$ROOT"
git submodule update --init --recursive
cmake --preset "$PRESET"
cmake --build --preset "$PRESET"
echo "Built: $ARTEFACTS"
