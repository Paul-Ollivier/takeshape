# Changelog

## [Unreleased] — M0 skeleton
- CMake project: JUCE 9.0.5 + ARA SDK 2.3.0 as pinned submodules; AU, VST3 and Standalone targets.
- ARA document controller with versioned archive header (v1, no payload yet).
- Pass-through ARA playback renderer (sample-accurate, mono/stereo, wait-free edit guard).
- Region inspector editor: selected region's name, start, length; regions rendered by this instance.
- Scripts: build, xcode, install, auval, pluginval, clean AU cache, test, CI.
- Catch2 unit tests for `RealtimeEditGuard`.
