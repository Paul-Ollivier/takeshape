# SPDX-License-Identifier: AGPL-3.0-or-later
cmake_minimum_required(VERSION 3.25)

project(Restyle VERSION 0.0.1 LANGUAGES C CXX OBJC OBJCXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# macOS 14+, Apple Silicon first. Override on the command line to add x86_64 later.
set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "Minimum macOS version")
if(NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Target architectures" FORCE)
endif()

option(RESTYLE_COPY_AFTER_BUILD "Copy AU/VST3 to ~/Library/Audio/Plug-Ins after each build" ON)
option(RESTYLE_BUILD_TESTS "Build unit tests (fetches Catch2)" ON)

# --- Dependencies (git submodules, pinned) ---------------------------------------------------
set(RESTYLE_EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

if(NOT EXISTS "${RESTYLE_EXTERNAL_DIR}/JUCE/CMakeLists.txt"
   OR NOT EXISTS "${RESTYLE_EXTERNAL_DIR}/ARA_SDK/ARA_API/ARAInterface.h")
    message(FATAL_ERROR "Submodules missing. Run: git submodule update --init --recursive")
endif()

add_subdirectory("${RESTYLE_EXTERNAL_DIR}/JUCE" EXCLUDE_FROM_ALL)
juce_set_ara_sdk_path("${RESTYLE_EXTERNAL_DIR}/ARA_SDK")   # must come before juce_add_plugin

include(cmake/CompilerWarnings.cmake)

add_subdirectory(src)

if(RESTYLE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
