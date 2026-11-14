#include "ara/DocumentController.h"
#include "ara/PlaybackRenderer.h"

namespace restyle
{

void DocumentController::willBeginEditing (juce::ARADocument*)
{
    editGuard.beginEdit();
}

void DocumentController::didEndEditing (juce::ARADocument*)
{
    editGuard.endEdit();
}

juce::ARAPlaybackRenderer* DocumentController::doCreatePlaybackRenderer() noexcept
{
    return new PlaybackRenderer (getDocumentController(), editGuard);
}

// Archive layout (v1): [int32 formatVersion][int64 numModifications = 0]
// ARA gotcha: the host may restore a *partial* document (filter) — e.g. Logic importing tracks
// from another project — so always look objects up via the filter, never assume all exist.
bool DocumentController::doRestoreObjectsFromStream (juce::ARAInputStream& input,
                                                     const juce::ARARestoreObjectsFilter*) noexcept
{
    const auto version = input.readInt();

    if (input.failed() || version < 1 || version > archiveFormatVersion)
        return false;   // newer archive than we understand: refuse rather than corrupt

    const auto numModifications = input.readInt64();
    juce::ignoreUnused (numModifications);  // M1: per-modification take state

    return ! input.failed();
}

bool DocumentController::doStoreObjectsToStream (juce::ARAOutputStream& output,
                                                 const juce::ARAStoreObjectsFilter*) noexcept
{
    return output.writeInt (archiveFormatVersion)
        && output.writeInt64 (0);
}

} // namespace restyle

//==============================================================================
// Required entry point for IS_ARA_EFFECT plugins (alongside createPluginFilter()).
const ARA::ARAFactory* JUCE_CALLTYPE createARAFactory()
{
    return juce::ARADocumentControllerSpecialisation::createARAFactory<restyle::DocumentController>();
}
