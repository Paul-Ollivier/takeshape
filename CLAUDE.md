# Restyle: notes for Claude Code

- The product brief and working agreement are in `CLAUDE_CODE_PROMPT-DAWIA.md`. Read it first.
- Current milestone: **M0** (see `CHANGELOG.md`). Stop and report at the end of each milestone.
- CMake is the source of truth. Never edit `build-xcode/*.xcodeproj`.
- Before adding any dependency, check its license and record it in `THIRD_PARTY.md`. Ask first for anything GPL/AGPL.
- Realtime rules: no allocations, locks or I/O on the audio thread. Read `docs/ARCHITECTURE.md` (threading) before touching `src/ara/`.
- Never store API keys anywhere but the macOS Keychain. Never log prompts or audio.
- Log decisions and host quirks in `docs/DEVLOG.md`.
- Build with `scripts/build.sh`; test with `scripts/test.sh`, `scripts/auval.sh` and `scripts/pluginval.sh`.
