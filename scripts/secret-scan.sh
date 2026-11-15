#!/usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# Usage: scripts/secret-scan.sh   — scans the full git history for secrets with gitleaks (also run in CI).
set -euo pipefail
cd "$(dirname "$0")/.."
exec gitleaks git . --redact --no-banner
