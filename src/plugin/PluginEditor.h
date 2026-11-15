// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "ui/RegionInspector.h"

namespace restyle
{

class PluginProcessor;

class PluginEditor final : public juce::AudioProcessorEditor,
                           public juce::AudioProcessorEditorARAExtension
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

    juce::AudioProcessorEditorARAExtension* getARAClientExtensions() override { return this; }

private:
    std::unique_ptr<RegionInspector> inspector;   // null when not hosted via ARA

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

} // namespace restyle
