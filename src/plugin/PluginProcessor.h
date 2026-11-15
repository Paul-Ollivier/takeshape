// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace takeshape
{

/**
    The per-instance AudioProcessor. In ARA mode it's a thin shell: all audio comes from the
    PlaybackRenderer bound to this instance via AudioProcessorARAExtension.

    Without an ARA host (Standalone, non-ARA hosts, pluginval's non-ARA tests) it outputs
    silence: Takeshape is region-based and has no meaningful insert-effect behaviour.
*/
class PluginProcessor final : public juce::AudioProcessor,
                              public juce::AudioProcessorARAExtension
{
public:
    PluginProcessor();
    ~PluginProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using juce::AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override;

    int getNumPrograms() override                             { return 1; }
    int getCurrentProgram() override                          { return 0; }
    void setCurrentProgram (int) override                     {}
    const juce::String getProgramName (int) override          { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    // Plugin (non-ARA) state: intentionally empty. Document state lives in the ARA archive.
    // NEVER put API keys here — they belong in the Keychain only.
    void getStateInformation (juce::MemoryBlock&) override {}
    void setStateInformation (const void*, int) override   {}

    juce::AudioProcessorARAExtension* getARAClientExtensions() override { return this; }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};

} // namespace takeshape
