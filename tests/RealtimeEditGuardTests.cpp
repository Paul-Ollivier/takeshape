#include <catch2/catch_test_macros.hpp>

#include "core/RealtimeEditGuard.h"

#include <atomic>
#include <thread>

using restyle::RealtimeEditGuard;

TEST_CASE ("RealtimeEditGuard: read succeeds when idle, fails during edit", "[rt]")
{
    RealtimeEditGuard guard;

    {
        auto scope = guard.tryRead();
        REQUIRE (scope.acquired());
    }

    guard.beginEdit();
    REQUIRE (guard.isEditing());
    REQUIRE_FALSE (guard.tryRead().acquired());

    guard.beginEdit();          // nested (host may nest edit cycles)
    guard.endEdit();
    REQUIRE_FALSE (guard.tryRead().acquired());

    guard.endEdit();
    REQUIRE (guard.tryRead().acquired());
}

TEST_CASE ("RealtimeEditGuard: beginEdit waits for in-flight reader", "[rt]")
{
    RealtimeEditGuard guard;
    std::atomic<bool> readerInside { false }, releaseReader { false }, editStarted { false };

    std::thread audio ([&]
    {
        auto scope = guard.tryRead();
        REQUIRE (scope.acquired());
        readerInside = true;
        while (! releaseReader) std::this_thread::yield();
    });

    while (! readerInside) std::this_thread::yield();

    std::thread editor ([&] { guard.beginEdit(); editStarted = true; });

    std::this_thread::sleep_for (std::chrono::milliseconds (20));
    REQUIRE_FALSE (editStarted.load());     // still blocked on the reader

    releaseReader = true;
    audio.join();
    editor.join();
    REQUIRE (editStarted.load());
    guard.endEdit();
}

TEST_CASE ("RealtimeEditGuard: no reader ever observes an edit in progress", "[rt][stress]")
{
    RealtimeEditGuard guard;
    std::atomic<bool> stop { false };
    std::atomic<int> sharedState { 0 };   // odd = mid-edit
    std::atomic<int> violations { 0 };

    std::thread audio ([&]
    {
        while (! stop)
            if (auto scope = guard.tryRead())
                if ((sharedState.load (std::memory_order_relaxed) & 1) != 0)
                    ++violations;
    });

    for (int i = 0; i < 20000; ++i)
    {
        guard.beginEdit();
        sharedState.fetch_add (1, std::memory_order_relaxed);
        sharedState.fetch_add (1, std::memory_order_relaxed);
        guard.endEdit();
    }

    stop = true;
    audio.join();
    REQUIRE (violations == 0);
}
