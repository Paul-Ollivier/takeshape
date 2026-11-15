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

## 2026-11-15 — Open-source pivot (AGPLv3)

**Decisions**
- Brief revised: free, open source, **AGPL-3.0-or-later**, public repo. JUCE is now used under its AGPLv3 option,
  so no commercial licence is needed. The old "flag GPL/AGPL" rule becomes "every dependency must be AGPLv3-compatible".
- VST3 SDK bundled in JUCE 9.0.5 is **3.8.0, MIT**: compatible.
- SPDX headers on every C++/CMake/shell file (after the shebang in scripts).
- Secret scan: gitleaks via a versioned `.githooks/pre-commit` (`git config core.hooksPath`) instead of the
  Python `pre-commit` framework, so there is no extra toolchain. CI runs a full-history scan on Linux (cheap) with a pinned release.
- History scan at this point: no leaks found.

## 2026-11-15 — Renamed to Takeshape

**Decisions**
- Name: **Takeshape** (one word, capital T). No audio product uses it; TakeShape is an unrelated headless-CMS
  company (owns takeshape.com/.io and the `takeshape` GitHub org), so trademark risk is low but plain web search is shared.
- Final plug-in identity, replacing the M0 placeholders above:
  - AU manufacturer `Tksh`, plug-in code `Tksp`, AU type `aufx`; company name shown in hosts: `Takeshape`.
  - Bundle ID `io.github.paul-ollivier.takeshape` (a domain we control: the GitHub Pages user domain).
  - ARA factory `io.github.paul-ollivier.takeshape.arafactory.<version>`, archive `io.github.paul-ollivier.takeshape.aradocumentarchive.1`.
  - **Don't change these again after users save projects**: hosts key saved plug-ins and ARA archives on them.
- Brief renamed to `CLAUDE_CODE_PROMPT.md`. C++ namespace `takeshape`; CMake options `TAKESHAPE_*`.
- Old `Restyle.component` / `Restyle.vst3` removed from `~/Library/Audio/Plug-Ins` so Logic doesn't list both.

**Gotcha**
- After renaming, delete `build/ninja-*` and `build-xcode`: old `Restyle_*` targets and caches linger otherwise.

## 2026-11-15 — Logic: ARA requires Rosetta → universal build

**Found while testing M0 in Logic Pro 13.0 (macOS 27.2.1, Apple Silicon)**
- Native Logic loaded Takeshape in `AUHostingServiceXPC_arrow` (out of process) as a plain effect: editor showed the
  non-ARA fallback, audio was silent. Logic only hosts ARA plug-ins when it runs **under Rosetta** (Apple support note,
  Celemony, Synchro Arts; reportedly unchanged in Logic 13). ARA SDK 2.3's generic IPC library is postponed, so there is no
  in-SDK path for out-of-process ARA either.
- Plug-in Manager shows two rows: "Takeshape" (Effect) and "Takeshape (ARA)" (ARA). There is **no** ARA menu on regions;
  insert on the track's first Audio FX slot. `docs/DEBUGGING.md` was wrong on both points; fixed.

**Decision**
- Build **universal (arm64 + x86_64)** by default so Rosetta Logic can load it. Costs roughly 2x compile time.
  `auval` passes natively and under `arch -x86_64`.

**Risk to track**
- Apple is winding down Rosetta in releases after macOS 27. If Logic still lacks native ARA by then, Logic + ARA stops
  working for everyone (Melodyne included). Watch Logic release notes; keep REAPER / Live 13 (native ARA) as fallback hosts.

**Open**
- The editor decides ARA vs fallback once, at construction. If a host opens the editor before binding the instance,
  it stays on the fallback text. Fix: re-check on a message-thread timer or when binding completes.

## Host differences
| Host | Format | ARA? | Notes |
|---|---|---|---|
| Logic Pro 13.0 | AU | Rosetta only | Native: loads out of process, no ARA. Two Plug-in Manager rows (Effect / ARA). Insert on track, no region ARA menu. |
| REAPER | VST3/AU | (to test) | |
| Live 13 beta | VST3/AU | (to test) | Arrangement View only; one ARA plug-in per project. |
