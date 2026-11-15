# Restyle (working title)

ARA 2 audio plug-in: select a region in your DAW, type a prompt, get AI-transformed alternate
takes of that region, in place. Model-agnostic, BYOK. Full brief: [`CLAUDE_CODE_PROMPT-DAWIA.md`](CLAUDE_CODE_PROMPT-DAWIA.md).

**Status:** M0 (toolchain skeleton): ARA pass-through playback plus a region inspector.

## Prerequisites (macOS 14+, Apple Silicon)

```sh
xcode-select --install          # or full Xcode from the App Store (required for the Xcode workflow)
brew install cmake ninja
brew install gitleaks            # secret scan (pre-commit hook + CI)
git submodule update --init --recursive   # JUCE 9.0.5 + ARA SDK 2.3.0
scripts/setup-hooks.sh           # enable the gitleaks pre-commit hook
```

## Build and run

| Task | Command |
|---|---|
| CLI build (Ninja, Debug) | `scripts/build.sh` (or `scripts/build.sh release`) |
| Unit tests | `scripts/test.sh` |
| Xcode project (generated, never hand-edit) | `scripts/xcode.sh` → `build-xcode/Restyle.xcodeproj` |
| Install AU + VST3, ad-hoc sign, reset AU cache | `scripts/install.sh` |
| Apple AU validation | `scripts/auval.sh` |
| pluginval (strictness 5) | `scripts/pluginval.sh 5` (downloads pluginval into `tools/` on first run) |
| Force AU rescan | `scripts/clean-au-cache.sh` |
| Secret scan (full history) | `scripts/secret-scan.sh` |
| Everything (CI) | `scripts/ci.sh` |

Debug builds copy AU/VST3 into `~/Library/Audio/Plug-Ins/` automatically
(`-DRESTYLE_COPY_AFTER_BUILD=OFF` to disable).

## Layout

```
CMakeLists.txt            root: options, submodules, JUCE + ARA SDK path
CMakePresets.json         ninja-debug / ninja-release / xcode
cmake/                    helper CMake (warnings)
external/JUCE, ARA_SDK    git submodules (pinned)
src/
  core/                   JUCE-free C++ (unit-tested): RealtimeEditGuard; later Job, TakeStore, Conform
  ara/                    DocumentController, PlaybackRenderer (ARA model + rendering)
  plugin/                 AudioProcessor + Editor shells
  ui/                     JUCE components (RegionInspector)
  (later) backends/       IBackend + Stability / ElevenLabs / Local / Suno stub
tests/                    Catch2 tests for src/core
scripts/                  build, install, auval, pluginval, AU cache, CI
docs/                     DEVLOG, ARCHITECTURE (threading), DEBUGGING, backends/
```

## Contributing

Commits follow [Conventional Commits](https://www.conventionalcommits.org/). Every source file starts with
`SPDX-License-Identifier: AGPL-3.0-or-later`. Never commit keys, signing identities or Team IDs;
the gitleaks hook and CI check every commit. A full `CONTRIBUTING.md` lands in M5.

## License

[AGPL-3.0-or-later](LICENSE). Third-party components and their licenses: [`THIRD_PARTY.md`](THIRD_PARTY.md).
