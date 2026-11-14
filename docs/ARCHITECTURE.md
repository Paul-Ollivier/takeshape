# Architecture and threading

See the brief (§4) for the target architecture. This file records **what runs where** as it's built.

## Threads

| Call | Thread | Notes |
|---|---|---|
| `createARAFactory`, DocumentController ctor/dtor | Host main | One DC per ARA document (Logic: per project). Outlives the plug-in instances bound to it. |
| `willBeginEditing` / `didEndEditing` | Host main | Bracket all model graph changes. We raise `RealtimeEditGuard` here. |
| `doCreate*`, `did/willUpdate*Properties`, `willDestroy*` | Host main | Only inside an edit bracket. |
| `doStoreObjectsToStream` / `doRestoreObjectsFromStream` | Host main | Archive I/O. Restore may be *partial* (filtered). |
| `ARAEditorView::Listener::onNewSelection` | Host main (JUCE message thread) | Selection in host UI. |
| `PlaybackRenderer::prepareToPlay` / `releaseResources` | Host main | Host never adds/removes regions while the renderer is prepared. |
| `PlaybackRenderer::processBlock` | Audio thread (realtime) **or** host render thread (offline bounce, `Realtime::no`) | See realtime rules below. |
| `BufferingAudioReader` refill | `SharedReadThread` (ours, high priority) | Pulls samples from the host via ARA audio access. |
| Future: job submission / polling / downloads | Worker pool (M1+) | Never touches the audio thread; hands finished takes over via lock-free swap. |

## Realtime guarantees (audio thread)

- **No allocations:** all buffers and readers are created in `prepareToPlay`. `std::map::find` doesn't allocate.
- **No blocking locks:** model reads are gated by `RealtimeEditGuard::tryRead()`, which is wait-free. During a host edit we output silence for that block.
- **No I/O:** sample data comes from `BufferingAudioReader` read-ahead, with a 0 ms timeout in realtime mode.
- **Known M0 gap:** `BufferingAudioReader::readSamples` takes a short internal `CriticalSection` (the same as JUCE's ARA demo). M1 replaces this with our own preloaded take buffers published via atomic pointer swap.
- How we'll enforce it: a debug-only `RealtimeAllocationGuard` (malloc hook) on the audio thread in M1, plus code review.
