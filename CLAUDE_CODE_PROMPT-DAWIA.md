# Project brief for Claude Code — "Restyle" (working title)

> Paste this whole file as your first message in Claude Code, or save it as `CLAUDE.md` at the repo root.

## 1. What we're building

An **ARA 2 audio plugin** (AU for Logic Pro first, VST3 second) that lets a producer
**select an audio region in their DAW, type a prompt, and get back alternate AI-transformed
takes of that region, in place**, aligned to the timeline, auditionable A/B/C, then committed.

Think "Suno Studio / ElevenLabs Music, but inside Logic", with **pluggable model backends**.
The plugin is model-agnostic: the value is the workflow (region-native, takes, tempo/length
conform, non-destructive), not any single model.

**This is a free, open-source project (AGPLv3), not a commercial product.** The goal is to
become the reference open-source integration of AI music models into DAWs: ship a convincing
demo fast, be easy to contribute to (especially new backends), and be visible. Optimize for
speed, clarity of code and docs, and contributor-friendliness over monetization.

I'm a senior C++ / UI engineer (25 years, games). Don't over-explain C++; do explain
ARA and JUCE-specific gotchas when they come up.

## 2. Non-negotiable constraints (legal + safety)

1. **No unofficial APIs.** Never use reverse-engineered Suno endpoints or third-party
   "Suno API" resellers. Suno is a *future* backend behind a partner agreement: build
   the interface slot and a stub, nothing more.
2. **Bring-your-own-key (BYOK).** Users paste their own provider API keys. Store keys in
   the **macOS Keychain** (never in plugin state, presets, project files or logs).
   **Never embed a developer key in the binary.**
3. **No backend server of our own** in v1. The plugin talks directly to providers.
   We never receive, store or log user audio. No telemetry containing audio or prompts.
4. A first-run **terms screen**: the user confirms they own or have rights to the audio they
   send, and that outputs are governed by the chosen provider's terms.
5. **Licensing:** the project is **AGPLv3** (this lets us use JUCE under its AGPLv3 option,
   no commercial JUCE license needed). Every dependency must be AGPLv3-compatible
   (MIT, BSD, ISC, Apache 2.0, GPLv3, AGPLv3 are fine). Track each one with its license in
   `THIRD_PARTY.md`. The ARA SDK is Apache 2.0. Check the VST3 SDK's current license and
   flag any incompatibility. Add SPDX headers (`// SPDX-License-Identifier: AGPL-3.0-or-later`)
   to every source file.
6. **Public repo hygiene:** the repo is public from day one. No secrets, keys, personal paths,
   signing identities or Team IDs in code, configs or history. Use `.env.example` /
   `config.example.json` patterns; add a pre-commit secret scan (e.g. gitleaks).

## 3. Tech stack

- **JUCE 8** (latest stable), **CMake** (not Projucer). CMake is the source of truth;
  generate the IDE project with `cmake -G Xcode -B build-xcode` and develop and debug in **Xcode**.
  Never hand-edit the generated `.xcodeproj`. Also keep a Ninja build for fast CLI builds.
- **Host priority:** 1) **Logic Pro** (primary target); 2) **Ableton Live 13** (adds ARA 2,
  public beta now, release early 2027; ARA works in Arrangement View only, and only **one ARA
  plugin per project**); 3) Reaper (fast debug host).
- **ARA SDK 2.x** (Celemony, github.com/Celemony/ARA_SDK) via JUCE's ARA support:
  `juce_set_ara_sdk_path(...)` and `IS_ARA_EFFECT TRUE` in `juce_add_plugin`.
- Formats: **AU (ARA)** first, **VST3 (ARA)** second. Standalone app target for UI dev.
- C++20. macOS 14+ / Apple Silicon first (Intel optional later).
- Networking: `juce::URL` / `WebInputStream` is acceptable for v1; if it's limiting for
  multipart + long polling, propose an alternative (e.g. libcurl) and ask first.
- Time-stretch for conform: **Signalsmith Stretch** (MIT) preferred. Rubber Band is GPL/commercial,
  so don't use it without asking.
- UI: native JUCE components first (fast, robust). We may move to a WebView UI later.
- Tests: Catch2 or JUCE UnitTest for non-ARA logic; **pluginval** and `auval` in CI scripts.
- Reference code: `magenta/the-infinite-crate` (Apache 2.0, JUCE + React + Lyria RealTime)
  is a useful reference for a cloud-model plugin's API key handling and async patterns.
  **Do not fork it**; it's Projucer-based and not ARA. Read it, borrow ideas, credit it if we copy code.

## 4. Architecture

```
DAW (Logic) ── ARA ──▶ DocumentController
                         ├─ AudioSource / AudioModification / PlaybackRegion model
                         ├─ RegionJobManager ──▶ JobQueue (worker threads, cancel, retry)
                         │                          └─ IBackend (strategy)
                         │                               ├─ StabilityBackend   (cloud, BYOK)
                         │                               ├─ ElevenLabsBackend  (cloud, BYOK)
                         │                               ├─ LocalBackend       (localhost helper, later)
                         │                               └─ SunoBackend        (stub only)
                         ├─ TakeStore (on-disk cache, content-hashed)
                         └─ Conform (trim / time-stretch to region length & tempo)
PlaybackRenderer ──▶ plays the active take for each region (sample-accurate)
EditorView ──▶ region inspector: prompt, backend, params, takes list, commit
```

### Key concepts
- **Job** = (source audio slice, prompt, backend id, params, seed) → 1..N takes.
  Jobs are async (seconds to minutes). Persist pending jobs and resume polling after reload.
- **Take** = rendered audio file + metadata (backend, model, prompt, params, provider job id,
  timestamp). Stored in a per-document cache folder; referenced by hash.
- **Non-destructive**: the original audio is always one tap away ("Take 0 = original").
- **Persistence**: region → takes mapping and active-take selection go into the ARA archive
  (store/restore document). Audio files live in the cache, not in the archive.
- **Commit**: renders the active take as the region's audio (in Logic the user then bounces
  in place). Document what each host supports.

### IBackend interface (draft; refine it)
```cpp
struct BackendCapabilities {
    bool generateFromPrompt;     // text → audio
    bool transformUserAudio;     // audio + prompt → audio (the core feature)
    bool inpaintRange;           // regenerate a sub-range, keep the rest
    bool exactDuration;          // honours requested length
    std::optional<int> maxSeconds;
};
class IBackend {
public:
    virtual ~IBackend() = default;
    virtual juce::String id() const = 0;
    virtual BackendCapabilities capabilities() const = 0;
    virtual JobHandle submit(const JobRequest&) = 0;      // non-blocking
    virtual JobStatus poll(const JobHandle&) = 0;          // or callback-based
    virtual void cancel(const JobHandle&) = 0;
};
```
The UI must **grey out actions a backend can't do** (from `capabilities()`), not fail late.

## 5. Backend facts — VERIFY BEFORE CODING

APIs change fast. Before implementing any backend, **fetch its current official docs**
(ElevenLabs publishes `https://elevenlabs.io/docs/llms.txt`) and write a short
`docs/backends/<name>.md` with endpoints, auth, limits, formats and terms notes.
Never invent request fields. If the docs and this brief disagree, the docs win; tell me.

What I know as of Nov 2026 (re-check all of it):
- **ElevenLabs Music API**: text → music (`POST /v1/music`, composition plans,
  `compose_detailed`, `store_for_inpainting`), model `music_v2_5`.
  **Inpainting works on songs ElevenLabs generated** (via stored `song_id`).
  **Uploading your own audio (`/v1/music/upload`) is enterprise-only.** So with a normal
  BYOK key, ElevenLabs can *generate* and *inpaint its own generations*, but
  **cannot transform arbitrary user audio**. Capabilities must reflect that, with an
  "enterprise" flag that unlocks upload.
- **Stability AI (Stable Audio)**: check whether the hosted API offers **audio-to-audio**
  (init audio + strength) for the current Stable Audio model, and its limits/terms.
  If yes, this is the **v1 "transform my region" backend**.
- **Local Stable Audio 3 (open weights)**: possible later via a localhost helper process
  (Python) that the plugin talks to over HTTP. Investigate audio-to-audio support and
  license terms; don't build it before M4.
- **Suno**: no public API (partner program only). Stub backend that returns
  "not available" with a link to the partner page. Nothing else.

## 6. Milestones (stop and report at the end of each one)

**M0 — Skeleton (prove the toolchain)**
- CMake project, JUCE 8 + ARA SDK, AU + VST3 + Standalone targets.
- Passes `auval -v aufx ...` and pluginval (strictness 5+).
- Loads in **Logic Pro as an ARA plugin** (appears in the region's ARA menu) and in Reaper.
  Then check the **Live 13 beta** (AU and VST3), and log any host differences in `docs/DEVLOG.md`.
- Debug setup: an Xcode scheme that launches Logic (or Reaper) as the executable, with
  instructions for attaching the debugger to the AU host process
  (Logic runs AUs out of process on Apple Silicon; document this).
- Plays the region's original audio through the ARA playback renderer (pass-through).
- ✅ Acceptance: I can insert it in Logic, see a region's name and length in the editor, and hear it unchanged.

**M1 — Region model + takes (no AI yet)**
- TakeStore + cache; a fake backend `ReverseBackend` that returns the region reversed
  after a 3 s delay (simulates async).
- Takes list UI: Original / Take A / B / C, audition and switch active take, delete.
- Persist/restore in the ARA archive; survives project save/reload.
- ✅ Acceptance: reload a Logic project and the active takes are still there.

**M2 — First real backend**
- Implement whichever backend can **transform user audio with a BYOK key** (likely Stability;
  confirm in step 5). Keychain key storage, settings panel, error UX (quota, auth, network,
  content refused), cancel, retry, progress.
- Conform: trim or pad to the region length; optional time-stretch to the exact length.
- ✅ Acceptance: in Logic, select an 8-bar loop, prompt "dusty 70s soul, Rhodes and brushed drums",
  get 2 takes back, aligned to the grid, A/B them.

**M3 — ElevenLabs backend**
- Generate-from-prompt into an empty or selected region (length = region length, tempo hint).
- Inpaint sub-ranges of regions that came *from* ElevenLabs (keep `song_id` in take metadata).
- ✅ Acceptance: generate an 8-bar part, then regenerate bars 5–8 only.

**M4 — Polish for the demo video**
- Sub-range selection inside a region (transform only bars 5–8).
- Prompt history and presets; seed / variation controls; "make 3 variations".
- Visual feedback in the editor: waveform per take, job state, cost or credit hint if the API exposes it.
- Investigate the local Stable Audio helper (spike only, write up findings).

**M5 — Open-source release**
- GitHub Actions CI: build AU + VST3 on macOS, unit tests, pluginval, artifacts on tag.
- Signed + notarized `.pkg` on GitHub Releases (signing secrets only in CI secrets; document
  how forks build unsigned). Homebrew cask is a nice-to-have.
- `README.md` with demo GIF/video, install, BYOK setup per provider, supported hosts and known
  limitations (e.g. Live 13: one ARA plugin per project, Arrangement View only).
- `CONTRIBUTING.md` + **`docs/ADDING_A_BACKEND.md`**: a step-by-step guide plus a minimal
  example backend, so contributors can add a model in an afternoon. Issue templates
  (bug / host compatibility / new backend), `CODE_OF_CONDUCT.md`, `SECURITY.md`
  (how to report key-leak or security issues).
- First-run terms screen; optional crash reporting that is **opt-in** and never includes user audio or prompts.
- Optional later: GitHub Sponsors link; paid convenience builds are *not* in scope now.

## 7. Working agreement

- Start by **reading this brief, then propose the repo layout and an M0 plan**. Wait for my OK.
- Code is read by strangers: clear module boundaries, a short doc comment on every public
  interface, no clever tricks without a comment. `IBackend` is the public contract for
  contributors, so keep it stable, versioned and documented.
- Use Conventional Commits; keep `main` always buildable.
- Small, reviewable commits; one milestone per branch; a short `CHANGELOG.md`.
- When something is ambiguous or costly to undo (dependency choice, threading model,
  persistence format), **ask me first** with 2–3 options and your recommendation.
- Realtime-safety: no allocations, locks or I/O on the audio thread. Say how you guarantee it.
- ARA threading rules matter: document which calls run on which thread.
- Keep a `docs/DEVLOG.md` with decisions and gotchas (especially Logic-specific ARA quirks).
- Write a `scripts/` folder: build, run pluginval, run auval, clean AU cache
  (`killall -9 AudioComponentRegistrar`), install to `~/Library/Audio/Plug-Ins/Components`.
- My Mac is set to English; give macOS / Logic menu names in English.

## 8. First task

1. Check prerequisites on my machine (Xcode, CMake, JUCE, ARA SDK); give me exact install commands for anything missing.
2. Propose the repo structure and the CMake setup for M0.
3. Implement M0 and tell me exactly how to test it in Logic Pro.
