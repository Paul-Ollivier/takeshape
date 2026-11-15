// SPDX-License-Identifier: AGPL-3.0-or-later
#include "plugin/PluginEditor.h"
#include "plugin/PluginProcessor.h"

namespace restyle
{

PluginEditor::PluginEditor (PluginProcessor& p)
    : juce::AudioProcessorEditor (&p),
      juce::AudioProcessorEditorARAExtension (&p)
{
    if (auto* editorView = getARAEditorView())
    {
        inspector = std::make_unique<RegionInspector> (*editorView, p.getPlaybackRenderer());
        addAndMakeVisible (*inspector);
    }

    // ARA gotcha: hosts embed ARA editors in their own UI (Logic: the plug-in window opened from
    // the region's ARA menu), and expect them to be resizable.
    setResizable (true, false);
    setResizeLimits (420, 240, 2400, 1600);
    setSize (640, 360);
}

void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1c1d21));

    if (! isARAEditorView())
    {
        g.setColour (juce::Colours::white.withAlpha (0.8f));
        g.setFont (juce::FontOptions (15.0f));
        g.drawFittedText ("Restyle works on audio regions via ARA.\n"
                          "Load it as an ARA plug-in (Logic Pro: select a region, then choose Restyle "
                          "from the region's ARA plug-in menu).",
                          getLocalBounds().reduced (24), juce::Justification::centred, 4);
    }
}

void PluginEditor::resized()
{
    if (inspector != nullptr)
        inspector->setBounds (getLocalBounds());
}

} // namespace restyle
