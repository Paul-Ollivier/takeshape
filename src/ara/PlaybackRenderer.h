#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>

#include "core/RealtimeEditGuard.h"

#include <map>
#include <memory>

namespace restyle
{

/** One background thread per process (ref-counted via SharedResourcePointer) that fills the
    BufferingAudioReaders of every renderer. Stops when the last renderer goes away, so it never
    outlives the plugin binary. */
struct SharedReadThread final : juce::TimeSliceThread
{
    SharedReadThread();
};

/**
    Renders the playback regions assigned to one plugin instance (Logic: the regions on the
    track the plugin is inserted on).

    M0: pass-through. Each region plays its audio source unchanged, sample-accurately placed on
    the song timeline. M1 swaps the source for the region's active take.

    Threading:
    - prepareToPlay / releaseResources: host main thread. The host must not add/remove playback
      regions while the renderer is prepared, so the reader map is stable during processBlock.
    - processBlock: audio thread (or a host render thread when bouncing offline: realtime == no).

    Realtime-safety notes (see docs/DEVLOG.md):
    - No allocation in processBlock: the temp buffer and readers are created in prepareToPlay;
      std::map::find doesn't allocate.
    - Reading the model graph is gated by RealtimeEditGuard (wait-free, never blocks).
    - KNOWN GAP (M0): juce::BufferingAudioReader::readSamples takes a CriticalSection briefly to
      look up its cached blocks. That's what JUCE's ARA demo ships, but it's a lock on the audio
      thread. M1 replaces source reading with our own preloaded take buffers.
*/
class PlaybackRenderer final : public juce::ARAPlaybackRenderer
{
public:
    PlaybackRenderer (ARA::PlugIn::DocumentController*, RealtimeEditGuard&);

    void prepareToPlay (double sampleRate, int maximumSamplesPerBlock, int numChannels,
                        juce::AudioProcessor::ProcessingPrecision, AlwaysNonRealtime) override;
    void releaseResources() override;

    bool processBlock (juce::AudioBuffer<float>&, juce::AudioProcessor::Realtime,
                       const juce::AudioPlayHead::PositionInfo&) noexcept override;

    using juce::ARAPlaybackRenderer::processBlock;

private:
    class ReaderHolder;

    RealtimeEditGuard& editGuard;
    juce::SharedResourcePointer<SharedReadThread> readThread;   // declared before readers: outlives them
    std::map<juce::ARAAudioSource*, std::unique_ptr<ReaderHolder>> readers;
    juce::AudioBuffer<float> mixBuffer;

    double sampleRate = 48000.0;
    int maxBlockSize = 0;
    int numChannels = 2;
    bool useBufferedReaders = true;
};

} // namespace restyle
