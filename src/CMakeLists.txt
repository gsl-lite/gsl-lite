# Copyright 2015-2019 by Martin Moene
# Copyright 2019-2025 by Moritz Beutel
#
# https://github.com/gsl-lite/gsl-lite
#
# This code is licensed under the MIT License (MIT).


cmake_minimum_required( VERSION 3.16 FATAL_ERROR )

# Interface library:
add_library( gsl-lite INTERFACE )
add_library( gsl::gsl-lite ALIAS gsl-lite )
add_library( gsl-lite::gsl-lite ALIAS gsl-lite )
target_include_directories( gsl-lite
    INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
)
target_sources( gsl-lite
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gsl-lite.natvis>"
)

# "gsl-lite-v1" target for modern defaults:
add_library( gsl-lite-v1 INTERFACE )
add_library( gsl::gsl-lite-v1 ALIAS gsl-lite-v1 )
add_library( gsl-lite::gsl-lite-v1 ALIAS gsl-lite-v1 )
target_include_directories( gsl-lite-v1
    INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_sources( gsl-lite-v1
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gsl-lite.natvis>"
)

# "gsl-lite-v0" target for legacy defaults:
add_library( gsl-lite-v0 INTERFACE )
add_library( gsl::gsl-lite-v0 ALIAS gsl-lite-v0 )
add_library( gsl-lite::gsl-lite-v0 ALIAS gsl-lite-v0 )
target_include_directories( gsl-lite-v0
    INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_compile_definitions( gsl-lite-v0
    INTERFACE
        gsl_CONFIG_DEFAULTS_VERSION=0
)
target_sources( gsl-lite-v0
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gsl-lite.natvis>"
)

# Installation:
install(
    DIRECTORY
        "${PROJECT_SOURCE_DIR}/include/" # the trailing '/' is significant
    TYPE INCLUDE
)
install(
    TARGETS      gsl-lite gsl-lite-v0 gsl-lite-v1
    EXPORT       gsl-lite
    INCLUDES
)
