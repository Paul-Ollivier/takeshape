#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Everything a CI job runs on macOS: build, unit tests, install, auval, pluginval.
set -euo pipefail
cd "$(dirname "$0")"
./build.sh "${1:-release}"
./test.sh "${1:-release}"
./install.sh "${1:-release}"
./auval.sh
./pluginval.sh 5
