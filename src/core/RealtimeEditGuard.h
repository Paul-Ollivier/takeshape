// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <atomic>
#include <thread>
#include <utility>

namespace restyle
{

/**
    Lets the audio thread skip rendering while the host edits the ARA document, without the
    audio thread ever blocking.

    - Audio thread: tryRead() is wait-free (two atomic RMWs and a load). If an edit is in
      progress it fails immediately and the caller outputs silence / bypasses for that block.
    - Edit thread (ARA willBeginEditing / didEndEditing, host main thread): beginEdit() raises the
      flag, then yields until in-flight readers drain. The wait is bounded by one audio block.

    This is a Dekker-style handshake; it relies on seq_cst ordering between "reader announces
    itself, then checks flag" and "writer raises flag, then checks readers". Don't weaken the
    memory orders without re-proving it.

    Replaces the juce::ReadWriteLock used by JUCE's ARAPluginDemo, whose tryEnterRead() takes an
    internal SpinLock (fine in practice, but not wait-free).
*/
class RealtimeEditGuard
{
public:
    class ReadScope
    {
    public:
        ReadScope() = default;
        explicit ReadScope (RealtimeEditGuard& g) noexcept : guard (&g) {}
        ReadScope (ReadScope&& other) noexcept : guard (std::exchange (other.guard, nullptr)) {}
        ReadScope& operator= (ReadScope&&) = delete;
        ReadScope (const ReadScope&) = delete;
        ReadScope& operator= (const ReadScope&) = delete;
        ~ReadScope() { if (guard != nullptr) guard->readers.fetch_sub (1, std::memory_order_release); }

        [[nodiscard]] bool acquired() const noexcept { return guard != nullptr; }
        explicit operator bool() const noexcept      { return acquired(); }

    private:
        RealtimeEditGuard* guard = nullptr;
    };

    /** Audio thread. Wait-free. */
    [[nodiscard]] ReadScope tryRead() noexcept
    {
        readers.fetch_add (1, std::memory_order_seq_cst);

        if (editDepth.load (std::memory_order_seq_cst) > 0)
        {
            readers.fetch_sub (1, std::memory_order_release);
            return {};
        }

        return ReadScope { *this };
    }

    /** Non-realtime thread. Nests. Returns once no audio-thread reader is inside a ReadScope. */
    void beginEdit() noexcept
    {
        editDepth.fetch_add (1, std::memory_order_seq_cst);

        while (readers.load (std::memory_order_seq_cst) != 0)
            std::this_thread::yield();
    }

    void endEdit() noexcept
    {
        editDepth.fetch_sub (1, std::memory_order_release);
    }

    [[nodiscard]] bool isEditing() const noexcept { return editDepth.load (std::memory_order_acquire) > 0; }

private:
    std::atomic<int> readers   { 0 };
    std::atomic<int> editDepth { 0 };
};

} // namespace restyle
