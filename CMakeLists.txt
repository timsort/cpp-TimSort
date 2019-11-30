cmake_minimum_required(VERSION 3.8.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(timsort VERSION 1.0.0 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# Project options
option(BUILD_TESTING "Build the tests" ON)
option(BUILD_BENCHMARKS "Build the benchmarks" OFF)

# Create gfx::timsort as an interface library
add_library(timsort INTERFACE)

target_include_directories(timsort INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(timsort INTERFACE cxx_std_11)

add_library(gfx::timsort ALIAS timsort)

# Install targets and files
install(
    TARGETS timsort
    EXPORT gfx-timsort-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    EXPORT gfx-timsort-targets
    NAMESPACE gfx::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gfx
)

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/gfx/timsort.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/gfx
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gfx-timsort-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/gfx-timsort-config.cmake
    INSTALL_DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/gfx
)

# Bypass automatic architeture check introduced by CMake,
# use the ARCH_INDEPENDENT option for this in the future
set(GFX_TIMSORT_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
unset(CMAKE_SIZEOF_VOID_P)
write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/cmake/gfx-timsort-config-version.cmake
    COMPATIBILITY
        SameMajorVersion
)
set(CMAKE_SIZEOF_VOID_P ${GFX_TIMSORT_SIZEOF_VOID_P})

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/gfx-timsort-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/gfx-timsort-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gfx
)

# Export target so that it can be used in subdirectories
export(
    EXPORT gfx-timsort-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/cmake/gfx-timsort-targets.cmake
    NAMESPACE gfx::
)

# Build tests and/or benchmarks if this is the main project
if (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    if (BUILD_TESTING)
        enable_testing()
        add_subdirectory(tests)
    endif()

    if (BUILD_BENCHMARKS)
        add_subdirectory(benchmarks)
    endif()
endif()
