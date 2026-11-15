// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <optional>
#include <vector>

namespace takeshape
{

/**
    M0 region inspector: shows the selected playback region's name, position and length, plus
    the regions this instance renders. Becomes the prompt / takes panel in M1+.

    Threading: every callback here runs on the message thread (ARA document + editor-view
    notifications are delivered on the host main thread, which JUCE treats as the message thread).
    We cache display strings rather than holding model pointers in paint(), and we drop the
    selected-region pointer in willDestroyPlaybackRegion, so nothing dangles.
*/
class RegionInspector final : public juce::Component,
                              private juce::ARAEditorView::Listener,
                              private juce::ARADocumentListener,
                              private juce::ARAPlaybackRegionListener
{
public:
    RegionInspector (juce::ARAEditorView&, juce::ARAPlaybackRenderer* rendererOrNull);
    ~RegionInspector() override;

    void paint (juce::Graphics&) override;

private:
    struct RegionInfo
    {
        juce::String name;
        double startSeconds = 0.0, durationSeconds = 0.0;
        juce::int64 durationSamples = 0;
        double sampleRate = 0.0;
        int channels = 0;
    };

    // ARAEditorView::Listener
    void onNewSelection (const juce::ARAViewSelection&) override;
    // ARADocumentListener
    void didEndEditing (juce::ARADocument*) override;
    // ARAPlaybackRegionListener
    void didUpdatePlaybackRegionProperties (juce::ARAPlaybackRegion*) override;
    void willDestroyPlaybackRegion (juce::ARAPlaybackRegion*) override;

    void setSelectedRegion (juce::ARAPlaybackRegion*);
    void refresh();
    static RegionInfo describe (const juce::ARAPlaybackRegion&);
    static juce::String formatTime (double seconds);

    juce::ARAEditorView& editorView;
    juce::ARAPlaybackRenderer* renderer;
    juce::ARADocument* document = nullptr;
    juce::ARAPlaybackRegion* selectedRegion = nullptr;

    std::optional<RegionInfo> selectedInfo;
    std::vector<RegionInfo> rendererRegions;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RegionInspector)
};

} // namespace takeshape
