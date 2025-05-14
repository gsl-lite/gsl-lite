# Copyright 2015-2019 by Martin Moene
# Copyright 2019-2025 by Moritz Beutel
#
# https://github.com/gsl-lite/gsl-lite
#
# This code is licensed under the MIT License (MIT).


cmake_minimum_required( VERSION 3.20 FATAL_ERROR )

project( my-program LANGUAGES CXX )

# Obtain source dependencies.
set( CPM_USE_LOCAL_PACKAGES ON )  # pass-thru mode to support external package managers
set( CPM_SOURCE_CACHE "${CMAKE_BINARY_DIR}/CPM-cache" )
include( cmake/CPM.cmake )
CPMAddPackage( NAME gsl-lite VERSION 1.0.0 GITHUB_REPOSITORY gsl-lite/gsl-lite )

find_package( gsl-lite 1.0 REQUIRED )

add_executable( my-program main.cpp )
target_compile_features( my-program PRIVATE cxx_std_17 )
target_link_libraries( my-program PRIVATE gsl-lite::gsl-lite )
