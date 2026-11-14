#include "ui/RegionInspector.h"

namespace restyle
{

RegionInspector::RegionInspector (juce::ARAEditorView& view, juce::ARAPlaybackRenderer* r)
    : editorView (view), renderer (r)
{
    if (auto* spec = juce::ARADocumentControllerSpecialisation::getSpecialisedDocumentController (editorView.getDocumentController()))
        document = spec->getDocument();

    editorView.addListener (this);

    if (document != nullptr)
        document->addListener (this);

    // Don't call editorView.getViewSelection() here: ARA asserts the editor is "open", which the
    // host signals separately from creating our component. The host re-sends the selection.
    refresh();
}

RegionInspector::~RegionInspector()
{
    setSelectedRegion (nullptr);

    if (document != nullptr)
        document->removeListener (this);

    editorView.removeListener (this);
}

//==============================================================================
void RegionInspector::onNewSelection (const juce::ARAViewSelection& selection)
{
    const auto& regions = selection.getPlaybackRegions<juce::ARAPlaybackRegion>();
    setSelectedRegion (regions.empty() ? nullptr : regions.front());
    refresh();
}

void RegionInspector::didEndEditing (juce::ARADocument*)                       { refresh(); }
void RegionInspector::didUpdatePlaybackRegionProperties (juce::ARAPlaybackRegion*) { refresh(); }

void RegionInspector::willDestroyPlaybackRegion (juce::ARAPlaybackRegion* region)
{
    if (region == selectedRegion)
        setSelectedRegion (nullptr);
}

void RegionInspector::setSelectedRegion (juce::ARAPlaybackRegion* region)
{
    if (region == selectedRegion)
        return;

    if (selectedRegion != nullptr)
        selectedRegion->removeListener (this);

    selectedRegion = region;

    if (selectedRegion != nullptr)
        selectedRegion->addListener (this);
}

//==============================================================================
RegionInspector::RegionInfo RegionInspector::describe (const juce::ARAPlaybackRegion& region)
{
    RegionInfo info;
    const auto* source = region.getAudioModification()->getAudioSource();

    if (const char* name = region.getEffectiveName())
        info.name = juce::String::fromUTF8 (name);

    if (info.name.isEmpty())
        info.name = "(unnamed region)";

    info.startSeconds    = region.getStartInPlaybackTime();
    info.durationSeconds = region.getDurationInPlaybackTime();
    info.sampleRate      = source->getSampleRate();
    info.channels        = source->getChannelCount();
    info.durationSamples = region.getDurationInPlaybackSamples (info.sampleRate);
    return info;
}

void RegionInspector::refresh()
{
    selectedInfo.reset();

    if (selectedRegion != nullptr)
        selectedInfo = describe (*selectedRegion);

    rendererRegions.clear();

    if (renderer != nullptr)
        for (auto* region : renderer->getPlaybackRegions())
            rendererRegions.push_back (describe (*region));

    repaint();
}

juce::String RegionInspector::formatTime (double seconds)
{
    const auto mins = (int) (seconds / 60.0);
    return juce::String (mins) + ":" + juce::String (seconds - mins * 60.0, 3).paddedLeft ('0', 6);
}

//==============================================================================
void RegionInspector::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1c1d21));
    auto area = getLocalBounds().reduced (20);

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (20.0f, juce::Font::bold));
    g.drawText ("Restyle", area.removeFromTop (28), juce::Justification::centredLeft);
    area.removeFromTop (12);

    auto line = [&] (const juce::String& text, float size, juce::Colour colour)
    {
        g.setColour (colour);
        g.setFont (juce::FontOptions (size));
        g.drawText (text, area.removeFromTop ((int) size + 8), juce::Justification::centredLeft, true);
    };

    const auto dim = juce::Colours::white.withAlpha (0.55f);

    if (selectedInfo)
    {
        const auto& r = *selectedInfo;
        line ("Selected region", 12.0f, dim);
        line (r.name, 17.0f, juce::Colours::white);
        line ("Length  " + formatTime (r.durationSeconds) + "   (" + juce::String (r.durationSamples) + " samples)",
              14.0f, juce::Colours::white);
        line ("Starts at  " + formatTime (r.startSeconds), 14.0f, juce::Colours::white);
        line (juce::String (r.sampleRate / 1000.0, 1) + " kHz, " + (r.channels == 1 ? "mono" : juce::String (r.channels) + " ch"),
              13.0f, dim);
    }
    else
    {
        line ("No region selected. Select a region in the arrangement.", 14.0f, dim);
    }

    area.removeFromTop (16);
    line ("Regions rendered by this instance: " + juce::String ((int) rendererRegions.size()), 12.0f, dim);

    for (const auto& r : rendererRegions)
    {
        if (area.getHeight() < 20)
            break;

        line ("  " + r.name + "  -  " + formatTime (r.startSeconds) + "  +" + formatTime (r.durationSeconds),
              13.0f, juce::Colours::white.withAlpha (0.8f));
    }
}

} // namespace restyle
