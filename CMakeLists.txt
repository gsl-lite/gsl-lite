# Copyright 2015-2018 by Martin Moene
#
# gsl-lite is based on GSL: Guideline Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT).

cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

# Disable building and performing of tests if gsl-lite is bundled as subproject:

if( DEFINED PROJECT_NAME )
  set( IS_SUBPROJECT ON )
endif()

include( CMakePackageConfigHelpers )

set( package_name "gsl-lite" )

project( ${package_name} VERSION 0.29.0 LANGUAGES CXX )

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

# Build and perform tests, unless this is a subproject:

if ( NOT IS_SUBPROJECT )
    enable_testing()
    add_subdirectory( test )
#   add_subdirectory( example )
endif()

# end of file
