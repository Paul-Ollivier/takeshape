# SPDX-License-Identifier: AGPL-3.0-or-later
include(FetchContent)
FetchContent_Declare(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.16.1      # BSL-1.0
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(Catch2)

add_executable(takeshape_tests
    RealtimeEditGuardTests.cpp)

target_link_libraries(takeshape_tests PRIVATE takeshape_core Catch2::Catch2WithMain)
takeshape_set_strict_warnings(takeshape_tests)

include(Catch)
catch_discover_tests(takeshape_tests)
