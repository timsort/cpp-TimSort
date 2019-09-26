include(DownloadProject)

# Download and configure Catch2 for the tests
download_project(PROJ Catch2
                 GIT_REPOSITORY https://github.com/catchorg/Catch2
                 GIT_TAG Catch1.x
                 UPDATE_DISCONNECTED 1
)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/contrib)

# Configure Catch2 so that it looks like a proper target
add_library(Catch2 INTERFACE)
add_library(Catch2::Catch2 ALIAS Catch2)
target_include_directories(Catch2 INTERFACE
    $<BUILD_INTERFACE:${Catch2_SOURCE_DIR}/single_include>  
    $<INSTALL_INTERFACE:include>
)

# Testsutie options
option(GFX_TIMSORT_USE_VALGRIND "Whether to run the tests with Valgrind" OFF)
set(GFX_TIMSORT_SANITIZE "" CACHE STRING "Comma-separated list of options to pass to -fsanitize")

macro(configure_tests target)
    # Add required dependencies to tests
    target_link_libraries(${target} PRIVATE
        Catch2::Catch2
        gfx::timsort
    )

    # Somewhat speed up Catch2 compile times
    target_compile_definitions(${target} PRIVATE
        CATCH_CONFIG_FAST_COMPILE
        CATCH_CONFIG_DISABLE_MATCHERS
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
        $<$<AND:$<CONFIG:DEBUG>,$<CXX_COMPILER_ID:Clang>>:-O0>
        $<$<AND:$<CONFIG:DEBUG>,$<CXX_COMPILER_ID:GNU>>:-Og>
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

# Tests that require C++11 support to run
add_executable(cxx_11_tests
    main.cpp
    cxx_11_tests.cpp
)
configure_tests(cxx_11_tests)
target_compile_features(cxx_11_tests PRIVATE cxx_std_11)

# Configure Valgrind
if (${GFX_TIMSORT_USE_VALGRIND})
    find_program(MEMORYCHECK_COMMAND valgrind)
    set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --track-origins=yes --error-exitcode=1 --show-reachable=no")
    if (APPLE)
        set(MEMORYCHECK_SUPPRESSIONS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/valgrind-osx.supp)
    endif()
endif()

include(CTest)
include(ParseAndAddCatchTests)

ParseAndAddCatchTests(cxx_98_tests)
ParseAndAddCatchTests(cxx_11_tests)
