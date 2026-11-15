#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Usage: scripts/pluginval.sh [strictness=5]  — validates the installed VST3 and AU.
# Note: pluginval loads the plug-in WITHOUT ARA, so it exercises the non-ARA fallback path
# (silence) plus state, bus layouts, threading and editor creation. ARA paths need a real host.
source "$(dirname "$0")/_common.sh"
STRICTNESS="${1:-5}"
[[ -x "$PLUGINVAL" ]] || "$(dirname "$0")/fetch-pluginval.sh"
status=0
for p in "$VST3_DIR/$PLUGIN_NAME.vst3" "$AU_DIR/$PLUGIN_NAME.component"; do
    echo "=== pluginval (strictness $STRICTNESS): $p"
    "$PLUGINVAL" --strictness-level "$STRICTNESS" --validate-in-process --skip-gui-tests=false \
                 --output-dir "$ROOT/build/pluginval" --validate "$p" || status=1
done
exit $status
