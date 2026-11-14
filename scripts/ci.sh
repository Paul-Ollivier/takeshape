#!/usr/bin/env bash
# Everything a CI job runs on macOS: build, unit tests, install, auval, pluginval.
set -euo pipefail
cd "$(dirname "$0")"
./build.sh "${1:-release}"
CONFIG="${1:-release}" ./test.sh "${1:-release}"
./install.sh "${1:-release}"
./auval.sh
./pluginval.sh 5
