# Dev log

## 2026-11-14 — M0 scaffold

**Decisions**
- **Dependencies as git submodules** under `external/` (JUCE, ARA SDK), pinned to tags. ARA SDK needs
  recursive submodules. Catch2 via FetchContent (test only). Easy to swap to CPM later.
- **JUCE 9.0.5** (approved over the brief's "JUCE 8"). Same AGPLv3/commercial dual licence (JUCE 9 EULA).
  JUCE 9's breaking changes didn't touch our code; it built clean with no changes.
- **ARA SDK 2.3.0**, the version JUCE 9.0.5 requires (`external/JUCE/docs/ARA.md`).
- **Test host:** `/Applications/Logic Pro.app` (perpetual licence), not Logic Pro Creator Studio.
- **Plug-in identity (placeholders, pending your decision):** manufacturer `Dawi`, plug-in `Rsty`, bundle `com.dawia.restyle`, AU type `aufx`.
  Changing the codes later orphans saved projects, so finalise before beta.
- **ARA archive ID** `com.dawia.restyle.aradocumentarchive.1` + an int32 format version inside the stream.
  Bump the ID only for incompatible formats; use the in-stream version for compatible evolution.
- **Edit guard:** replaced the demo's `ReadWriteLock` with a wait-free `RealtimeEditGuard` (unit-tested).
- **Non-ARA behaviour:** silence. The plug-in is meaningless as a plain insert.
- Arm64 only, macOS 14 deployment target.

**Gotchas hit**
- JUCE CMake: `ARA_FACTORY_ID` / `ARA_DOCUMENT_ARCHIVE_ID` are pasted verbatim into a `#define`,
  so they need escaped quotes: `"\"com.foo...\""`.
- `ARAEditorView::getViewSelection()` asserts unless the host has opened the editor. Don't call it from
  the editor constructor; wait for `onNewSelection`.
- `JUCE_DISPLAY_SPLASH_SCREEN` no longer exists in JUCE 8+ (it warns). Removed.
- **Submodule pinning:** `git submodule add` records the remote's default-branch HEAD, not the tag you
  check out afterwards, and `git submodule update` then silently reverts to it. After checking out a tag,
  `git add external/<dep>` to record it. Verify with `git submodule status`.

**Verified (CLI)**
- Ninja Debug build on JUCE 9.0.5: OK (AU, VST3, Standalone). Unit tests: 3/3 pass.
- `auval -strict -v aufx Rsty Dawi`: **AU VALIDATION SUCCEEDED**.
- Xcode project generates; the `Restyle_AU` scheme launches `/Applications/Logic Pro.app`.

**Not yet verified (needs manual run)**
- Logic Pro: ARA load, inspector, pass-through audio.
- REAPER: not installed on this machine.
- Ableton Live 13 beta: only Live 12 is installed (no ARA support in 12).

## 2026-11-14 — CI + pluginval

**Verified (CLI, clean build at the new repo path `~/Developer/DAWIA`)**
- `scripts/ci.sh release`: build, 3/3 unit tests, install, `auval -strict` (AU VALIDATION SUCCEEDED),
  pluginval strictness 5 on VST3 and AU: **SUCCESS** for both. Debug build: same result.
- pluginval AU warning "Current program is -1... Is this correct?" is benign: the processor reports
  1 program / current 0; the -1 comes from JUCE's AU wrapper (no factory presets). Revisit if we add presets.

**Added**
- GitHub Actions workflow `.github/workflows/ci.yml`: one `macos-15` job running `scripts/ci.sh release`
  on PRs, pushes to `main` and manual dispatch. Private repo: macOS minutes bill at 10x, so it's a single job
  with in-progress runs cancelled on new pushes.

**Gotchas hit**
- Moving the repo invalidates CMake caches (`CMakeCache.txt` stores the absolute source path). Delete
  `build/ninja-*` and `build-xcode` and regenerate.
- macOS `/bin/bash` is 3.2: with `set -u`, `"${arr[@]}"` on an empty array errors. Use `${arr[@]+"${arr[@]}"}`.
- `scripts/test.sh` always ran the `ninja-debug` ctest preset; it now follows the build config
  (added a `ninja-release` test preset).

## Host differences
| Host | Format | ARA? | Notes |
|---|---|---|---|
| Logic Pro | AU | (to test) | |
| REAPER | VST3/AU | (to test) | |
| Live 13 beta | VST3/AU | (to test) | Arrangement View only; one ARA plug-in per project. |
