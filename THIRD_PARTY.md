# Third-party dependencies

The project is AGPL-3.0-or-later. Every dependency must be AGPLv3-compatible (MIT, BSD, ISC, Apache 2.0, BSL-1.0, GPLv3, AGPLv3).

| Dependency | Version | License | Linked / shipped? | Notes |
|---|---|---|---|---|
| JUCE | 9.0.5 (submodule) | AGPLv3 (used under this option of its AGPLv3 / commercial dual licence) | Yes, statically linked | Compatible: the project itself is AGPL-3.0-or-later, so no commercial licence is needed. Terms: https://juce.com/legal/juce-9-licence/ |
| ARA SDK (ARA_API, ARA_Library) | 2.3.0 (submodule) | Apache 2.0 | Yes (ARA_Library compiled in via JUCE) | Keep `NOTICE.txt` attribution in the shipped product/docs. |
| VST3 SDK (bundled in JUCE) | 3.8.0 (bundled in JUCE 9.0.5) | MIT | Yes | Compatible. Checked `external/JUCE/modules/juce_audio_processors_headless/format_types/VST3_SDK/LICENSE.txt` (2026-11-15). The \"VST\" name and logo are Steinberg trademarks, covered separately by Steinberg's usage guidelines. |
| Catch2 | v3.16.1 (FetchContent) | BSL-1.0 | No (tests only) | |
| pluginval | v1.0.4 (downloaded to `tools/`) | GPLv3 | No: a test tool, never linked or redistributed | Compatible either way. |
| gitleaks | v8.30.1 (Homebrew locally, pinned release in CI) | MIT | No: a dev/CI tool | Secret scan. |

## Planned (not yet added)
| Dependency | License | Notes |
|---|---|---|
| Signalsmith Stretch | MIT | Time-stretch for conform (M2). |
| Rubber Band | GPLv2+ / commercial | GPLv3-compatible via \"or later\", but still ask first (the brief prefers Signalsmith). |
| magenta/the-infinite-crate | Apache 2.0 | Reference only. Credit it here if any code is copied. |
