// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "core/RealtimeEditGuard.h"

namespace takeshape
{

/**
    ARA document controller specialisation: the plugin's per-document "brain".

    Threading (ARA 2 / JUCE):
    - All ARA model graph calls (willBeginEditing, didEndEditing, object creation/updates/destruction,
      store/restore) arrive on the host's *main/document thread*. Model objects are only mutated
      between willBeginEditing and didEndEditing.
    - Playback renderers' processBlock runs on the *audio thread* and reads the model graph. We use
      RealtimeEditGuard so the audio thread never reads the graph mid-edit, and never blocks.
    - One DocumentController exists per ARA document (Logic: per project). It outlives all plugin
      instances (AudioProcessors) bound to it.

    M0: no custom audio-modification state; the archive stores only a format header.
    M1 will add region -> takes mapping + active take here.
*/
class DocumentController final : public juce::ARADocumentControllerSpecialisation
{
public:
    using ARADocumentControllerSpecialisation::ARADocumentControllerSpecialisation;

    RealtimeEditGuard& getEditGuard() noexcept { return editGuard; }

    static constexpr juce::int32 archiveFormatVersion = 1;

protected:
    void willBeginEditing (juce::ARADocument*) override;
    void didEndEditing (juce::ARADocument*) override;

    juce::ARAPlaybackRenderer* doCreatePlaybackRenderer() noexcept override;

    bool doRestoreObjectsFromStream (juce::ARAInputStream&, const juce::ARARestoreObjectsFilter*) noexcept override;
    bool doStoreObjectsToStream (juce::ARAOutputStream&, const juce::ARAStoreObjectsFilter*) noexcept override;

private:
    RealtimeEditGuard editGuard;
};

} // namespace takeshape
