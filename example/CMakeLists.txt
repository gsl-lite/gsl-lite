# Copyright 2015-2019 by Martin Moene
# Copyright 2019-2025 by Moritz Beutel
#
# https://github.com/gsl-lite/gsl-lite
#
# This code is licensed under the MIT License (MIT).

cmake_minimum_required( VERSION 3.20 FATAL_ERROR )

project( gsl-lite-example LANGUAGES CXX )

message( STATUS "Subproject '${PROJECT_NAME}'")

add_executable( 01-basic
    "01-basic.cpp"
)
target_compile_features( 01-basic INTERFACE cxx_std_11 )
target_link_libraries( 01-basic
    PRIVATE
        gsl-lite::gsl-lite
)

add_executable( 02-span
    "02-span.cpp"
)
target_compile_features( 02-span INTERFACE cxx_std_11 )
target_link_libraries( 02-span
    PRIVATE
        gsl-lite::gsl-lite
)
