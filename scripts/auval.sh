#!/usr/bin/env bash
# Validate the installed AU with Apple's auval (what Logic runs on first scan).
source "$(dirname "$0")/_common.sh"
"$(dirname "$0")/clean-au-cache.sh" >/dev/null
auval -a | grep -i "$AU_MANUFACTURER" || { echo "AU not registered — run scripts/install.sh"; exit 1; }
auval -strict -v "$AU_TYPE" "$AU_SUBTYPE" "$AU_MANUFACTURER"
