# Copyright 2015-2019 by Martin Moene
# Copyright 2019-2025 by Moritz Beutel
#
# https://github.com/gsl-lite/gsl-lite
#
# This code is licensed under the MIT License (MIT).


cmake_minimum_required( VERSION 3.16 FATAL_ERROR )

project( gsl-lite-examples LANGUAGES CXX )

message( STATUS "Subproject '${PROJECT_NAME}'")

add_executable( 01-basic
    "01-basic.cpp"
)
target_compile_features( 01-basic PRIVATE cxx_std_11 )
set_target_properties( 01-basic PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO )
target_link_libraries( 01-basic
    PRIVATE
        gsl-lite::gsl-lite
)

add_executable( 02-span
    "02-span.cpp"
)
target_compile_features( 02-span PRIVATE cxx_std_11 )
set_target_properties( 02-span PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO )
target_link_libraries( 02-span
    PRIVATE
        gsl-lite::gsl-lite
)
