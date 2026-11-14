include(FetchContent)
FetchContent_Declare(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.16.1      # BSL-1.0
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(Catch2)

add_executable(restyle_tests
    RealtimeEditGuardTests.cpp)

target_link_libraries(restyle_tests PRIVATE restyle_core Catch2::Catch2WithMain)
restyle_set_strict_warnings(restyle_tests)

include(Catch)
catch_discover_tests(restyle_tests)
