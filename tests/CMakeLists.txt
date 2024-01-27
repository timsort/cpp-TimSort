# Copyright (c) 2019-2024 Morwenn
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.24.0)

include(FetchContent)

# Test suite options
option(GFX_TIMSORT_USE_VALGRIND "Whether to run the tests with Valgrind" OFF)
set(GFX_TIMSORT_SANITIZE "" CACHE STRING "Comma-separated list of options to pass to -fsanitize")

# Find/download Catch2
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG v3.5.0
    SYSTEM
    FIND_PACKAGE_ARGS 3.1.0
)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

# Configure Valgrind
if (${GFX_TIMSORT_USE_VALGRIND})
    find_program(MEMORYCHECK_COMMAND valgrind)
    set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --track-origins=yes --error-exitcode=1 --show-reachable=no")
endif()

macro(configure_tests target)
    # Add required dependencies to tests
    target_link_libraries(${target} PRIVATE
        Catch2::Catch2WithMain
        gfx::timsort
    )

    target_compile_definitions(${target} PRIVATE
        # Somewhat speed up Catch2 compile times
        CATCH_CONFIG_FAST_COMPILE
        # Enable assertions for more thorough tests
        GFX_TIMSORT_ENABLE_ASSERT
    )

    # Add warnings
    if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wcast-align -Wmissing-declarations -Wmissing-include-dirs
            -Wnon-virtual-dtor -Wodr -Wpedantic -Wredundant-decls -Wundef -Wunreachable-code
            $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op -Wuseless-cast>
        )
    elseif (MSVC)
        target_compile_options(${target} PRIVATE /W4)
    endif()

    # Configure optimization options
    if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
        target_compile_options(${target} PRIVATE
            $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:Clang>>:-O0>
            $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>:-Og>
        )
    endif()

    # Use lld or the gold linker if possible
    if (UNIX AND NOT APPLE)
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            set_property(TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS " -fuse-ld=lld")
        else()
            set_property(TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS " -fuse-ld=gold")
        endif()
    endif()

    # Optionally enable sanitizers
    if (UNIX AND GFX_TIMSORT_SANITIZE)
        target_compile_options(${target} PRIVATE
            -fsanitize=${GFX_TIMSORT_SANITIZE}
            -fno-sanitize-recover=all
        )
        set_property(TARGET ${target}
            APPEND_STRING PROPERTY LINK_FLAGS
                " -fsanitize=${GFX_TIMSORT_SANITIZE}"
        )
    endif()
endmacro()

# Tests that can run with C++98
add_executable(cxx_98_tests
    cxx_98_tests.cpp
)
configure_tests(cxx_98_tests)
target_compile_features(cxx_98_tests PRIVATE cxx_std_98)

# Tests requiring C++11 support
add_executable(cxx_11_tests
    merge_cxx_11_tests.cpp
    cxx_11_tests.cpp
)
configure_tests(cxx_11_tests)
target_compile_features(cxx_11_tests PRIVATE cxx_std_11)

# Tests requiring C++17 support
add_executable(cxx_17_tests
    cxx_17_tests.cpp
)
configure_tests(cxx_17_tests)
target_compile_features(cxx_17_tests PRIVATE cxx_std_17)

# Tests requiring C++20 support
add_executable(cxx_20_tests
    cxx_20_tests.cpp
)
configure_tests(cxx_20_tests)
target_compile_features(cxx_20_tests PRIVATE cxx_std_20)

# Windows-specific tests
if (WIN32)
    add_executable(windows_tests
        windows.cpp
    )
    configure_tests(windows_tests)
    target_compile_features(windows_tests PRIVATE cxx_std_98)
endif()

include(CTest)
include(Catch)

string(RANDOM LENGTH 5 ALPHABET 123456789 RNG_SEED)
catch_discover_tests(cxx_98_tests EXTRA_ARGS --rng-seed ${RNG_SEED})
catch_discover_tests(cxx_11_tests EXTRA_ARGS --rng-seed ${RNG_SEED})
catch_discover_tests(cxx_17_tests EXTRA_ARGS --rng-seed ${RNG_SEED})
catch_discover_tests(cxx_20_tests EXTRA_ARGS --rng-seed ${RNG_SEED})
if (WIN32)
    catch_discover_tests(windows_tests EXTRA_ARGS --rng-seed ${RNG_SEED})
endif()
