# Copyright 2015-2019 by Martin Moene
# Copyright 2019-2025 by Moritz Beutel
#
# https://github.com/gsl-lite/gsl-lite
#
# gsl-lite is originally based on Microsoft GSL, which is an implementation of the C++ Core Guidelines Support Library:
# https://github.com/microsoft/GSL
#
# This code is licensed under the MIT License (MIT).


cmake_minimum_required( VERSION 3.16 FATAL_ERROR )

project(
    gsl-lite
    VERSION 1.0.0
    DESCRIPTION "defensive programming utilities for C++98, C++11 up"
    HOMEPAGE_URL "https://github.com/gsl-lite/gsl-lite"
    LANGUAGES CXX
)
message( STATUS "Project '${PROJECT_NAME}', version: '${${PROJECT_NAME}_VERSION}'" )

# Define build options.
# Do not build tests or examples by default.
option( GSL_LITE_OPT_BUILD_TESTS                 "Build and perform gsl-lite tests"                 OFF )
option( GSL_LITE_OPT_BUILD_CUDA_TESTS            "Build and perform gsl-lite CUDA tests"            OFF )
option( GSL_LITE_OPT_BUILD_EXAMPLES              "Build gsl-lite examples"                          OFF )
option( GSL_LITE_OPT_BUILD_STATIC_ANALYSIS_DEMOS "Build and perform gsl-lite static analysis demos" OFF )

# Include target definitions.
add_subdirectory( src )

# If requested, build and perform tests, build examples.
if ( GSL_LITE_OPT_BUILD_TESTS )
    set( ENABLE_TESTING ON )
    include( CTest )
    add_subdirectory( test )
endif()
if ( GSL_LITE_OPT_BUILD_CUDA_TESTS )
    set( ENABLE_TESTING ON )
    include( CTest )
    add_subdirectory( test/cuda )
endif()

if ( GSL_LITE_OPT_BUILD_EXAMPLES )
    add_subdirectory( example )
endif()

# Install.
include( cmake/InstallBasicPackageFiles.cmake )
install_basic_package_files( gsl-lite
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
    INCLUDE_CONTENT
"
# Define legacy aliases for imported targets.
if( NOT TARGET gsl::gsl-lite )
    add_library( gsl::gsl-lite ALIAS gsl-lite::gsl-lite )
endif()
if( NOT TARGET gsl::gsl-lite-v0 )
    add_library( gsl::gsl-lite-v0 ALIAS gsl-lite::gsl-lite-v0 )
endif()
if( NOT TARGET gsl::gsl-lite-v1 )
    add_library( gsl::gsl-lite-v1 ALIAS gsl-lite::gsl-lite-v1 )
endif()
"
)
