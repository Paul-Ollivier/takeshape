# Debugging

## Xcode
1. `scripts/xcode.sh` generates `build-xcode/Takeshape.xcodeproj` and opens it.
2. Choose the scheme:
   - **Takeshape_AU**: Run launches `/Applications/Logic Pro.app` (set by `TAKESHAPE_DEBUG_HOST`).
   - **Takeshape_VST3**: same host setting; point it at REAPER or Live 13 for VST3.
   - **Takeshape_Standalone**: UI work without a host (no ARA, so it shows the "load as ARA" message).
3. Building copies the AU/VST3 into `~/Library/Audio/Plug-Ins/` (`TAKESHAPE_COPY_AFTER_BUILD`).

Change the host: `TAKESHAPE_DEBUG_HOST="/Applications/REAPER.app" scripts/xcode.sh`.
Test host is **`/Applications/Logic Pro.app`** (the perpetual licence). `Logic Pro Creator Studio.app`
is also installed but isn't used for testing.

## Logic Pro specifics (verified 2026-11-15, Logic Pro 13.0, macOS 27.2)
- **ARA needs Logic under Rosetta.** Native Logic on Apple Silicon hosts AUs in a separate process
  (`AUHostingServiceXPC_arrow`) and offers no ARA there: Takeshape loads as a plain effect (fallback text, silence).
  Quit Logic, select `/Applications/Logic Pro.app` in Finder, **File > Get Info**, tick **Open using Rosetta**, relaunch.
  Our AU/VST3 are universal (arm64 + x86_64) for this reason.
- **Two entries.** Plug-in Manager lists **Takeshape** (type Effect, plain AU) and **Takeshape (ARA)** (type ARA). Use the (ARA) one.
- **Where ARA shows up.** There is no ARA menu on regions. Select the audio track, then in its channel strip
  (Inspector **I** or Mixer **X**) put **Audio Units > Takeshape > Takeshape (ARA)** in the **first Audio FX slot**.
  The plug-in receives every region on that track. Press Play once so Logic transfers the regions, then click a region.
- **Region selection.** Turn on **Logic Pro > Settings > General > Editing > Select track on region/marquee selection**
  so selecting a region also selects its track, which is what drives the plug-in view.
- **Debugging.** Under Rosetta the plug-in runs inside Logic's own (x86_64) process, so Xcode can attach to Logic directly.
  Hardened runtime may refuse the attach; then debug in REAPER and confirm behaviour in Logic.
- **AU cache.** After changing AU metadata (codes, name, version), quit Logic, run
  `scripts/clean-au-cache.sh`, and use **Logic Pro > Settings > Plug-in Manager > Reset & Rescan Selection** if needed.

## Validation
- `scripts/auval.sh`: Apple's validator. It must pass, or Logic disables the AU.
- `scripts/pluginval.sh 5`: runs **without ARA**, so it covers the non-ARA fallback, threading, state and editor.
