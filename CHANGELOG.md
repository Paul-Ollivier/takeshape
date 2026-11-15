# Changelog

## [Unreleased] — M0 skeleton
- CMake project: JUCE 9.0.5 + ARA SDK 2.3.0 as pinned submodules; AU, VST3 and Standalone targets.
- ARA document controller with versioned archive header (v1, no payload yet).
- Pass-through ARA playback renderer (sample-accurate, mono/stereo, wait-free edit guard).
- Region inspector editor: selected region's name, start, length; regions rendered by this instance.
- Scripts: build, xcode, install, auval, pluginval, clean AU cache, test, CI.
- Catch2 unit tests for `RealtimeEditGuard`.
- GitHub Actions CI (macOS, release): build, unit tests, auval, pluginval strictness 5.
- Fixed: `test.sh` ignored the release config; `xcode.sh` failed on macOS bash 3.2 without `TAKESHAPE_DEBUG_HOST`.
- Project licensed AGPL-3.0-or-later: `LICENSE`, SPDX headers on all source/CMake/script files, `THIRD_PARTY.md` re-checked (VST3 SDK 3.8.0 bundled in JUCE is MIT).
- Secret scanning: gitleaks pre-commit hook (`scripts/setup-hooks.sh`), `scripts/secret-scan.sh`, CI job.
- Brief updated to the open-source version.
- Renamed to **Takeshape**: AU `aufx Tksp Tksh`, bundle `io.github.paul-ollivier.takeshape`, matching ARA factory/archive IDs.
- Universal build (arm64 + x86_64): Logic Pro hosts ARA plug-ins only under Rosetta. `docs/DEBUGGING.md` corrected for Logic.
