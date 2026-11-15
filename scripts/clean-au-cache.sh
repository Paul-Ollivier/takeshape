#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Force macOS to rescan Audio Units. Quit Logic first.
# AudioComponentRegistrar caches component lists; killing it makes the next host launch rescan.
killall -9 AudioComponentRegistrar 2>/dev/null || true
echo "AudioComponentRegistrar reset. If Logic still shows a stale version: Logic Pro > Settings >"
echo "Plug-in Manager > select Takeshape > Reset & Rescan Selection."
