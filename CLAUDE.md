# Takeshape: notes for Claude Code

- The product brief and working agreement are in `CLAUDE_CODE_PROMPT.md`. Read it first.
- Current milestone: **M0** (see `CHANGELOG.md`). Stop and report at the end of each milestone.
- CMake is the source of truth. Never edit `build-xcode/*.xcodeproj`.
- The project is open source (AGPL-3.0-or-later) and the repo is public. Before adding any dependency, check that its license is AGPLv3-compatible and record it in `THIRD_PARTY.md`. Ask first for anything not clearly compatible.
- Every source file starts with an `SPDX-License-Identifier: AGPL-3.0-or-later` comment.
- No secrets, personal paths, signing identities or Team IDs in code, configs or history. Run `scripts/setup-hooks.sh` once per clone (gitleaks pre-commit).
- Conventional Commits; keep `main` always buildable.
- Realtime rules: no allocations, locks or I/O on the audio thread. Read `docs/ARCHITECTURE.md` (threading) before touching `src/ara/`.
- Never store API keys anywhere but the macOS Keychain. Never log prompts or audio.
- Log decisions and host quirks in `docs/DEVLOG.md`.
- Build with `scripts/build.sh`; test with `scripts/test.sh`, `scripts/auval.sh` and `scripts/pluginval.sh`.
