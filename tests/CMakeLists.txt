include(DownloadProject)

# Download and configure Catch2 for the tests
download_project(PROJ Catch2
                 GIT_REPOSITORY https://github.com/catchorg/Catch2
                 GIT_TAG master
                 UPDATE_DISCONNECTED 1
)
add_subdirectory(${Catch2_SOURCE_DIR} ${Catch2_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/contrib)

# Testsutie options
option(GFX_TIMSORT_USE_VALGRIND "Whether to run the tests with Valgrind" OFF)
set(GFX_TIMSORT_SANITIZE "" CACHE STRING "Comma-separated list of options to pass to -fsanitize")

# Configure Valgrind
if (${GFX_TIMSORT_USE_VALGRIND})
    find_program(MEMORYCHECK_COMMAND valgrind)
    set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --track-origins=yes --error-exitcode=1 --show-reachable=no")
    if (APPLE)
        set(MEMORYCHECK_SUPPRESSIONS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/valgrind-osx.supp)
    endif()
endif()

macro(configure_tests target)
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

# Tests that can run with C++98
add_executable(cxx_98_tests
    main.cpp
    cxx_98_tests.cpp
)
configure_tests(cxx_98_tests)
target_compile_features(cxx_98_tests PRIVATE cxx_std_98)

# Tests requiring C++11 support
add_executable(cxx_11_tests
    main.cpp
    cxx_11_tests.cpp
)
configure_tests(cxx_11_tests)
target_compile_features(cxx_11_tests PRIVATE cxx_std_11)

# Tests requiring C++17 support
add_executable(cxx_17_tests
    main.cpp
    cxx_17_tests.cpp
)
configure_tests(cxx_17_tests)
target_compile_features(cxx_17_tests PRIVATE cxx_std_17)

# Windows-specific tests
if (WIN32)
    add_executable(windows_tests
        main.cpp
        windows.cpp
    )
    configure_tests(windows_tests)
    target_compile_features(windows_tests PRIVATE cxx_std_98)
endif()

include(CTest)
include(Catch)

catch_discover_tests(cxx_98_tests)
catch_discover_tests(cxx_11_tests)
catch_discover_tests(cxx_17_tests)
if (WIN32)
    catch_discover_tests(windows_tests)
endif()
