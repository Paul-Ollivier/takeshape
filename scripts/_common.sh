# SPDX-License-Identifier: AGPL-3.0-or-later
# Sourced by the other scripts. Keep plugin identity in sync with src/CMakeLists.txt.
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PLUGIN_NAME="Restyle"
AU_TYPE="aufx"
AU_SUBTYPE="Rsty"          # PLUGIN_CODE
AU_MANUFACTURER="Dawi"     # PLUGIN_MANUFACTURER_CODE
CONFIG="${CONFIG:-debug}"  # debug | release
PRESET="ninja-${CONFIG}"
CMAKE_CONFIG="$(tr '[:lower:]' '[:upper:]' <<< "${CONFIG:0:1}")${CONFIG:1}"
ARTEFACTS="$ROOT/build/$PRESET/src/${PLUGIN_NAME}_artefacts/$CMAKE_CONFIG"
AU_DIR="$HOME/Library/Audio/Plug-Ins/Components"
VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
PLUGINVAL="$ROOT/tools/pluginval.app/Contents/MacOS/pluginval"
