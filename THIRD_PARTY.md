# Third-party dependencies

| Dependency | Version | License | Linked / shipped? | Notes |
|---|---|---|---|---|
| JUCE | 9.0.5 (submodule) | **AGPLv3 / JUCE 9 EULA** (dual) | Yes, statically linked | ⚠️ **A JUCE commercial licence is required before any closed-source sale.** Until then the project is effectively AGPLv3. Terms: https://juce.com/legal/juce-9-licence/ |
| ARA SDK (ARA_API, ARA_Library) | 2.3.0 (submodule) | Apache 2.0 | Yes (ARA_Library compiled in via JUCE) | Keep `NOTICE.txt` attribution in the shipped product/docs. |
| VST3 SDK (bundled in JUCE) | via JUCE | MIT (since VST3 SDK 3.8) | Yes | Check JUCE's bundled copy version before release. |
| Catch2 | v3.16.1 (FetchContent) | BSL-1.0 | No (tests only) | |
| pluginval | v1.0.4 (downloaded to `tools/`) | **GPLv3** | **No**: a test tool, never linked or redistributed | Fine for CI use. |

## Planned (not yet added)
| Dependency | License | Notes |
|---|---|---|
| Signalsmith Stretch | MIT | Time-stretch for conform (M2). |
| Rubber Band | **GPL / commercial** | ⚠️ Do not use without explicit approval. |
| magenta/the-infinite-crate | Apache 2.0 | Reference only. Credit it here if any code is copied. |
