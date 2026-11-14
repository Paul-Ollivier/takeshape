# Debugging

## Xcode
1. `scripts/xcode.sh` generates `build-xcode/Restyle.xcodeproj` and opens it.
2. Choose the scheme:
   - **Restyle_AU**: Run launches `/Applications/Logic Pro.app` (set by `RESTYLE_DEBUG_HOST`).
   - **Restyle_VST3**: same host setting; point it at REAPER or Live 13 for VST3.
   - **Restyle_Standalone**: UI work without a host (no ARA, so it shows the "load as ARA" message).
3. Building copies the AU/VST3 into `~/Library/Audio/Plug-Ins/` (`RESTYLE_COPY_AFTER_BUILD`).

Change the host: `RESTYLE_DEBUG_HOST="/Applications/REAPER.app" scripts/xcode.sh`.
Test host is **`/Applications/Logic Pro.app`** (the perpetual licence). `Logic Pro Creator Studio.app`
is also installed but isn't used for testing.

## Logic Pro specifics (⚠️ verify on this machine; record results in DEVLOG)
- **Out-of-process AUs.** Per the brief, Logic on Apple Silicon may host AUs outside its own process.
  If breakpoints don't hit when Xcode launches Logic, the plug-in is running in a helper process. Use
  **Debug > Attach to Process by PID or Name…** in Xcode, and find the process with
  `ps aux | grep -i -E "AUHostingService|Logic"` *after* inserting the plug-in.
- **Hardened runtime.** Attaching a debugger to a notarized, hardened app can be refused. If Xcode
  can't attach to Logic or its helper, debug in **REAPER** (easiest), then confirm behaviour in Logic.
- **AU cache.** After changing AU metadata (codes, name, version), quit Logic, run
  `scripts/clean-au-cache.sh`, and use **Logic Pro > Settings > Plug-in Manager > Reset & Rescan Selection** if needed.
- **Where ARA shows up.** On an audio track: Mixer channel strip **Audio FX** slot > **Audio Units > DAWIA > Restyle**.
  Logic then lists the plug-in for the track's regions. ARA plug-ins also appear in the region's
  **ARA** submenu (from the Control-click menu on a region) once they're on the track.

## Validation
- `scripts/auval.sh`: Apple's validator. It must pass, or Logic disables the AU.
- `scripts/pluginval.sh 5`: runs **without ARA**, so it covers the non-ARA fallback, threading, state and editor.
