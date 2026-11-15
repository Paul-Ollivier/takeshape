// SPDX-License-Identifier: AGPL-3.0-or-later
#include "ara/PlaybackRenderer.h"

namespace restyle
{

SharedReadThread::SharedReadThread() : juce::TimeSliceThread ("Restyle ARA sample reader")
{
    startThread (Priority::high);   // above normal so playback is fluent, below realtime
}

//==============================================================================
class PlaybackRenderer::ReaderHolder
{
public:
    ReaderHolder (juce::ARAAudioSource* source, bool buffered, int readAheadSamples,
                  juce::TimeSliceThread& thread)
    {
        auto direct = std::make_unique<juce::ARAAudioSourceReader> (source);

        if (buffered)
        {
            auto b = std::make_unique<juce::BufferingAudioReader> (direct.release(), thread, readAheadSamples);
            bufferedReader = b.get();
            reader = std::move (b);
        }
        else
        {
            reader = std::move (direct);
        }
    }

    void setReadTimeout (int ms) noexcept
    {
        if (bufferedReader != nullptr)
            bufferedReader->setReadTimeout (ms);
    }

    juce::AudioFormatReader& get() noexcept { return *reader; }

private:
    std::unique_ptr<juce::AudioFormatReader> reader;
    juce::BufferingAudioReader* bufferedReader = nullptr;
};

//==============================================================================
PlaybackRenderer::PlaybackRenderer (ARA::PlugIn::DocumentController* dc, RealtimeEditGuard& guard)
    : juce::ARAPlaybackRenderer (dc), editGuard (guard)
{
}

void PlaybackRenderer::prepareToPlay (double newSampleRate, int maximumSamplesPerBlock, int newNumChannels,
                                      juce::AudioProcessor::ProcessingPrecision, AlwaysNonRealtime alwaysNonRealtime)
{
    sampleRate   = newSampleRate;
    maxBlockSize = maximumSamplesPerBlock;
    numChannels  = newNumChannels;
    useBufferedReaders = (alwaysNonRealtime == AlwaysNonRealtime::no);

    mixBuffer.setSize (numChannels, maxBlockSize);
    readers.clear();

    const auto readAhead = juce::jmax (4 * maxBlockSize, juce::roundToInt (2.0 * sampleRate));

    for (auto* region : getPlaybackRegions())
    {
        auto* source = region->getAudioModification()->getAudioSource();

        if (! readers.contains (source))
            readers.emplace (source, std::make_unique<ReaderHolder> (source, useBufferedReaders, readAhead, *readThread));
    }
}

void PlaybackRenderer::releaseResources()
{
    readers.clear();
    mixBuffer.setSize (0, 0);
}

bool PlaybackRenderer::processBlock (juce::AudioBuffer<float>& buffer,
                                     juce::AudioProcessor::Realtime realtime,
                                     const juce::AudioPlayHead::PositionInfo& position) noexcept
{
    const auto numSamples = buffer.getNumSamples();
    buffer.clear();

    const auto scope = editGuard.tryRead();

    if (! scope)
        return true;    // host is editing the document: output silence for this block

    if (! position.getIsPlaying() || numSamples > maxBlockSize || buffer.getNumChannels() != numChannels)
        return true;

    const auto songStart  = position.getTimeInSamples().orFallback (0);
    const auto blockRange = juce::Range<juce::int64>::withStartAndLength (songStart, numSamples);
    bool success = true;

    for (auto* region : getPlaybackRegions())
    {
        // Song-time range covered by the region (head/tail ignored: we don't extend regions).
        const auto regionSongRange = region->getSampleRange (sampleRate, juce::ARAPlaybackRegion::IncludeHeadAndTail::no);
        auto renderRange = blockRange.getIntersectionWith (regionSongRange);

        if (renderRange.isEmpty())
            continue;

        // Map song time -> audio-modification (source) time. No time-stretch in M0, so it's an offset.
        const juce::Range<juce::int64> modRange { region->getStartInAudioModificationSamples(),
                                                  region->getEndInAudioModificationSamples() };
        const auto songToSource = modRange.getStart() - regionSongRange.getStart();
        renderRange = renderRange.getIntersectionWith (modRange.movedToStartAt (regionSongRange.getStart()));

        if (renderRange.isEmpty())
            continue;

        auto* source = region->getAudioModification()->getAudioSource();
        const auto it = readers.find (source);

        // M0 limitation: no sample-rate conversion. Mono<->stereo is handled by
        // AudioFormatReader::read (dupes mono to both channels); >2 channels must match exactly.
        const bool channelsOk = (numChannels <= 2 && source->getChannelCount() <= 2)
                             || source->getChannelCount() == numChannels;

        if (it == readers.end() || ! juce::exactlyEqual (source->getSampleRate(), sampleRate) || ! channelsOk)
        {
            success = false;
            continue;
        }

        auto& holder = *it->second;
        holder.setReadTimeout (realtime == juce::AudioProcessor::Realtime::no ? 100 : 0);

        const auto len           = (int) renderRange.getLength();
        const auto startInBuffer = (int) (renderRange.getStart() - blockRange.getStart());
        const auto startInSource = renderRange.getStart() + songToSource;

        // Overlapping regions on one track are summed (matches what Logic plays without the plugin
        // only for non-overlapping regions; Logic itself plays the topmost — revisit if needed).
        if (! holder.get().read (&mixBuffer, startInBuffer, len, startInSource, true, true))
        {
            success = false;
            continue;
        }

        for (int ch = 0; ch < numChannels; ++ch)
            buffer.addFrom (ch, startInBuffer, mixBuffer, ch, startInBuffer, len);
    }

    return success;
}

} // namespace restyle
