# Copyright 2015-2018 by Martin Moene
#
# https://github.com/martinmoene/gsl-lite
#
# gsl-lite is based on GSL: Guidelines Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT).

cmake_minimum_required( VERSION 3.5 FATAL_ERROR )

# gsl-lite project and version, updated by script/update-version.py:

project(
    gsl_lite
    VERSION 0.35.5
#   DESCRIPTION "A single-file header-only version of ISO C++ Guideline Support Library (GSL) for C++98, C++11 and later"
#   HOMEPAGE_URL "https://github.com/martinmoene/gsl-lite"
    LANGUAGES CXX )

# Package information:

set( unit_name       "gsl" )
set( package_nspace  "gsl" )
set( package_name    "${unit_name}-lite" )
set( package_version "${${PROJECT_NAME}_VERSION}" )

message( STATUS "Project '${PROJECT_NAME}', package '${package_name}' version: '${package_version}'" )

# Toplevel or subproject:

if ( CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME )
    set( gsl_IS_TOPLEVEL_PROJECT TRUE )
else()
    set( gsl_IS_TOPLEVEL_PROJECT FALSE )
endif()

# If toplevel project, enable building and performing of tests, disable building of examples:

option( GSL_LITE_OPT_BUILD_TESTS             "Build and perform gsl-lite tests" ${gsl_IS_TOPLEVEL_PROJECT} )
option( GSL_LITE_OPT_BUILD_EXAMPLES          "Build gsl-lite examples" OFF )
option( CMAKE_EXPORT_PACKAGE_REGISTRY        "Export build directory to CMake user package registry" OFF )
option( GSL_LITE_OPT_INSTALL_COMPAT_HEADER   "Install MS-GSL compatibility header <gsl/gsl>" OFF )
option( GSL_LITE_OPT_INSTALL_LEGACY_HEADERS  "Install legacy headers <gsl.h>, <gsl.hpp>, <gsl/gsl-lite.h>" OFF )

# If requested, build and perform tests, build examples:

if ( GSL_LITE_OPT_BUILD_TESTS )
    enable_testing()
    add_subdirectory( test )
endif()

if ( GSL_LITE_OPT_BUILD_EXAMPLES )
    enable_testing()
    add_subdirectory( example )
endif()

#
# Interface, installation and packaging
#

# CMake helpers:

include( GNUInstallDirs )
include( CMakePackageConfigHelpers )

# Interface library:

add_library(
    ${package_name} INTERFACE )
add_library(
    ${package_nspace}::${package_name} ALIAS ${package_name} )
target_include_directories(
    ${package_name}
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )
target_sources(
    ${package_name}
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gsl-lite.natvis>" )

# Interface library for targets with versioned defaults:

# "gsl-lite-v0" target for legacy defaults:
add_library(
    ${package_name}-v0 INTERFACE )
add_library(
    ${package_nspace}::${package_name}-v0 ALIAS ${package_name}-v0 )
target_include_directories(
    ${package_name}-v0
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )
target_sources(
    ${package_name}-v0
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gsl-lite.natvis>" )

# "gsl-lite-v1" target for modern defaults:
add_library(
    ${package_name}-v1 INTERFACE )
add_library(
    ${package_nspace}::${package_name}-v1 ALIAS ${package_name}-v1 )
target_include_directories(
    ${package_name}-v1
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )
target_compile_definitions(
    ${package_name}-v1
    INTERFACE
        gsl_CONFIG_DEFAULTS_VERSION=1)
target_sources(
    ${package_name}-v1
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gsl-lite.natvis>" )

# Package configuration:
# Note: package_name and package_target are used in package_config_in

set( package_folder         "${package_name}" )
set( package_target         "${package_name}-targets" )
set( package_config         "${package_name}-config.cmake" )
set( package_config_in      "${package_name}-config.cmake.in" )
set( package_config_version "${package_name}-config-version.cmake" )

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${package_config_in}"
    "${CMAKE_CURRENT_BINARY_DIR}/${package_config}"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${package_folder}"
)

configure_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${package_config_version}.in"
   "${CMAKE_CURRENT_BINARY_DIR}/${package_config_version}" @ONLY
)

# Installation:

install(
    TARGETS      ${package_name} ${package_name}-v0 ${package_name}-v1
    EXPORT       ${package_target}
#   INCLUDES DESTINATION "${...}"  # already set via target_include_directories()
)

install(
    EXPORT       ${package_target}
    NAMESPACE    ${package_nspace}::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${package_folder}"
)

export(
    EXPORT       ${package_target}
    NAMESPACE    ${package_nspace}::
    FILE         "${CMAKE_CURRENT_BINARY_DIR}/${package_name}-targets.cmake"
)

if(CMAKE_EXPORT_PACKAGE_REGISTRY)
    export(
        PACKAGE  ${package_name}
    )
endif()

install(
    FILES       "${CMAKE_CURRENT_BINARY_DIR}/${package_config}"
                "${CMAKE_CURRENT_BINARY_DIR}/${package_config_version}"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${package_folder}"
)

install(
    FILES       "include/gsl/gsl-lite.hpp"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/gsl/"
)
install(
    FILES       "include/gsl-lite/gsl-lite.hpp"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/gsl-lite/"
)
if(GSL_LITE_OPT_INSTALL_COMPAT_HEADER)
    install(
        FILES       "include/gsl/gsl"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/gsl/"
    )
endif()
if(GSL_LITE_OPT_INSTALL_LEGACY_HEADERS)
    install(
        FILES       "include/gsl/gsl-lite.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/gsl/"
    )
    install(
        FILES       "include/gsl.h"
                    "include/gsl.hpp"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/"
    )
endif()

# end of file
