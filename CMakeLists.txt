# Copyright 2015-2018 by Martin Moene
#
# https://github.com/martinmoene/gsl-lite
#
# gsl-lite is based on GSL: Guidelines Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT).

cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

# gsl-lite project and version, updated by script/update-version.py:

project(
    gsl_lite
    VERSION 0.32.0
    DESCRIPTION "A single-file header-only version of ISO C++ Guideline Support Library (GSL) for C++98, C++11 and later"
#   HOMEPAGE_URL "https://github.com/martinmoene/gsl-lite"
    LANGUAGES CXX )

set( ${PROJECT_NAME}_version "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" )

# Toplevel or subproject:

if ( CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME )
    set( gsl_IS_TOPLEVEL_PROJECT TRUE )
else()
    set( gsl_IS_TOPLEVEL_PROJECT FALSE )
endif()

# If toplevel project, enable building and performing of tests, disable building of examples:

option( GSL_LITE_OPT_BUILD_TESTS    "Build and perform gsl-lite tests" ${gsl_IS_TOPLEVEL_PROJECT} )
option( GSL_LITE_OPT_BUILD_EXAMPLES "Build gsl-lite examples" OFF )

include( CMakePackageConfigHelpers )

set( package_name "gsl-lite" )

set(         include_source_dir "${PROJECT_SOURCE_DIR}/include" )
set(        include_install_dir "include" )
set( package_config_install_dir "lib/cmake/${PROJECT_NAME}" )
set(        package_config_file "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" )
set(       package_version_file "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" )
set(        package_export_name "${PROJECT_NAME}Targets" )
set(          package_namespace "${PROJECT_NAME}::" )

# Interface library:

add_library(
    ${package_name} INTERFACE )

target_include_directories(
    ${package_name} INTERFACE "$<BUILD_INTERFACE:${include_source_dir}>" )

target_sources(
    ${package_name} INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/gsl-lite.natvis>" )

# Package configuration:

configure_package_config_file(
   "${PROJECT_SOURCE_DIR}/cmake/PackageConfig.cmake.in"
    ${package_config_file} INSTALL_DESTINATION ${package_config_install_dir} )

write_basic_package_version_file(
    ${package_version_file} COMPATIBILITY SameMajorVersion )

# Installation:

install(
    TARGETS     ${package_name}
    EXPORT      ${package_export_name}
    INCLUDES DESTINATION ${include_install_dir} )

install(
    EXPORT      ${package_export_name}
    NAMESPACE   ${package_namespace}
    DESTINATION ${package_config_install_dir} )

install(
    DIRECTORY  "${include_source_dir}/"
    DESTINATION ${include_install_dir} )

install(
    FILES      "${package_config_file}"
               "${package_version_file}"
    DESTINATION ${package_config_install_dir} )

# If requested, build and perform tests, build examples:

enable_testing()

if ( GSL_LITE_OPT_BUILD_TESTS )
    add_subdirectory( test )
endif()

if ( GSL_LITE_OPT_BUILD_EXAMPLES )
    add_subdirectory( example )
endif()

# end of file
