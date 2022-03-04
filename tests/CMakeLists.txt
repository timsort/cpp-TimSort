# Copyright (c) 2019-2022 Morwenn
# SPDX-License-Identifier: MIT

include(DownloadProject)

# Download and configure Catch2 for the tests
download_project(PROJ Catch2
                 GIT_REPOSITORY https://github.com/catchorg/Catch2
                 GIT_TAG v2.x
                 UPDATE_DISCONNECTED 1
)
add_subdirectory(${Catch2_SOURCE_DIR} ${Catch2_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/contrib)

# Test suite options
option(GFX_TIMSORT_USE_VALGRIND "Whether to run the tests with Valgrind" OFF)
set(GFX_TIMSORT_SANITIZE "" CACHE STRING "Comma-separated list of options to pass to -fsanitize")

# Configure Valgrind
if (${GFX_TIMSORT_USE_VALGRIND})
    find_program(MEMORYCHECK_COMMAND valgrind)
    set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --track-origins=yes --error-exitcode=1 --show-reachable=no")
endif()

macro(configure_tests target prefix)
    # Add required dependencies to tests
    target_link_libraries(${target} PRIVATE
        Catch2::Catch2
        gfx::timsort
    )

    target_compile_definitions(${target} PRIVATE
        # Somewhat speed up Catch2 compile times
        CATCH_CONFIG_FAST_COMPILE
        CATCH_CONFIG_DISABLE_MATCHERS
        # Enable assertions for more thorough tests
        GFX_TIMSORT_ENABLE_ASSERT
        # Decide which sorts to test
        GFX_TIMSORT_TEST_SORT=gfx::${prefix}sort
        GFX_TIMSORT_TEST_MERGE=gfx::${prefix}merge
    )

    # Add warnings
    if (NOT MSVC)
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wcast-align -Winline -Wmissing-declarations -Wmissing-include-dirs
            -Wnon-virtual-dtor -Wodr -Wpedantic -Wredundant-decls -Wundef -Wunreachable-code
            $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op -Wuseless-cast -Wzero-as-null-pointer-constant>
        )
    endif()

    # Configure optimization options
    target_compile_options(${target} PRIVATE
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:Clang>>:-O0>
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>:-Og>
    )

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

foreach(prefix tim adaptive_shivers_ power)
    # Tests that can run with C++98
    add_executable("${prefix}-cxx_98_tests"
        main.cpp
        cxx_98_tests.cpp
    )
    configure_tests("${prefix}-cxx_98_tests" ${prefix})
    target_compile_features("${prefix}-cxx_98_tests" PRIVATE cxx_std_98)

    # Tests requiring C++11 support
    add_executable("${prefix}-cxx_11_tests"
        main.cpp
        merge_cxx_11_tests.cpp
        cxx_11_tests.cpp
    )
    configure_tests("${prefix}-cxx_11_tests" ${prefix})
    target_compile_features("${prefix}-cxx_11_tests" PRIVATE cxx_std_11)

    # Tests requiring C++17 support
    add_executable("${prefix}-cxx_17_tests"
        main.cpp
        cxx_17_tests.cpp
    )
    configure_tests("${prefix}-cxx_17_tests" ${prefix})
    target_compile_features("${prefix}-cxx_17_tests" PRIVATE cxx_std_17)

    # Windows-specific tests
    if (WIN32)
        add_executable("${prefix}-windows_tests"
            main.cpp
            windows.cpp
        )
        configure_tests("${prefix}-windows_tests" ${prefix})
        target_compile_features("${prefix}-windows_tests" PRIVATE cxx_std_98)
    endif()

    include(CTest)
    include(Catch)

    string(RANDOM LENGTH 5 ALPHABET 0123456789 RNG_SEED)
    catch_discover_tests("${prefix}-cxx_98_tests" EXTRA_ARGS --rng-seed ${RNG_SEED})
    catch_discover_tests("${prefix}-cxx_11_tests" EXTRA_ARGS --rng-seed ${RNG_SEED})
    catch_discover_tests("${prefix}-cxx_17_tests" EXTRA_ARGS --rng-seed ${RNG_SEED})
    if (WIN32)
        catch_discover_tests("${prefix}-windows_tests" EXTRA_ARGS --rng-seed ${RNG_SEED})
    endif()
endforeach()
