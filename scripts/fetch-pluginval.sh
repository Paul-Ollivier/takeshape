#!/usr/bin/env bash
# Download Tracktion's pluginval (GPLv3 *tool* — used only for testing, never linked or shipped).
source "$(dirname "$0")/_common.sh"
VERSION="${PLUGINVAL_VERSION:-v1.0.4}"
mkdir -p "$ROOT/tools"
tmp="$(mktemp -d)"
curl -fsSL -o "$tmp/pluginval.zip" "https://github.com/Tracktion/pluginval/releases/download/$VERSION/pluginval_macOS.zip"
rm -rf "$ROOT/tools/pluginval.app"
unzip -q "$tmp/pluginval.zip" -d "$ROOT/tools"
rm -rf "$tmp"
"$PLUGINVAL" --version || true
