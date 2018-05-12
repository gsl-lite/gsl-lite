# Copyright 2015-2018 by Martin Moene
#
# gsl-lite is based on GSL: Guideline Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT).

cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

# gsl-lite version, updated by script/update-version.py:

set( gsl_lite_version "0.32.0" )

# At default, disable building and performing of tests and building of examples:

option( GSL_LITE_OPT_BUILD_TESTS    "Build and perform gsl-lite tests" OFF )
option( GSL_LITE_OPT_BUILD_EXAMPLES "Build gsl-lite examples" OFF )

include( CMakePackageConfigHelpers )

set( package_name "gsl-lite" )

project( ${package_name} VERSION ${gsl_lite_version} LANGUAGES CXX )

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
