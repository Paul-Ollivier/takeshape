#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Usage: scripts/setup-hooks.sh   — points git at the versioned hooks in .githooks/ (gitleaks secret scan).
set -euo pipefail
cd "$(dirname "$0")/.."
git config core.hooksPath .githooks
echo "Git hooks enabled (.githooks). Requires gitleaks: brew install gitleaks"
