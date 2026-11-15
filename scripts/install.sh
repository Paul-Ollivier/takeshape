#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Usage: scripts/install.sh [debug|release]
# Copies AU + VST3 into ~/Library/Audio/Plug-Ins, ad-hoc signs them (required on Apple Silicon),
# and resets the AU cache so Logic rescans.
CONFIG="${1:-${CONFIG:-debug}}"
source "$(dirname "$0")/_common.sh"

for pair in "AU/$PLUGIN_NAME.component:$AU_DIR" "VST3/$PLUGIN_NAME.vst3:$VST3_DIR"; do
    src="$ARTEFACTS/${pair%%:*}"; dst="${pair##*:}"
    [[ -d "$src" ]] || { echo "Missing $src — run scripts/build.sh $CONFIG first"; exit 1; }
    mkdir -p "$dst"
    rm -rf "$dst/$(basename "$src")"
    cp -R "$src" "$dst/"
    codesign --force --deep --sign - "$dst/$(basename "$src")"
    echo "Installed $(basename "$src") -> $dst"
done

"$(dirname "$0")/clean-au-cache.sh"
