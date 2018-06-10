# Copyright 2015-2018 by Martin Moene
#
# gsl-lite is based on GSL: Guideline Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT).

cmake_minimum_required( VERSION 3.1 FATAL_ERROR )

project( gsl-lite-example LANGUAGES CXX )

# Target default options and definitions:

set( OPTIONS "" )
set( DEFINITIONS "" )

# Sources (.cpp) and their base names:

set( SOURCES 
    01-basic.cpp 
    02-span.cpp 
)

string( REPLACE ".cpp" "" BASENAMES "${SOURCES}" )

# Function to create a target:

function( make_target source )
    string( REPLACE ".cpp" "" target "${source}" )
    add_executable            ( ${target} ${source}  )
    target_compile_options    ( ${target} PRIVATE ${OPTIONS} )
    target_compile_definitions( ${target} PRIVATE ${DEFINITIONS} )
    target_link_libraries     ( ${target} PRIVATE gsl-lite )
    set_target_properties     ( ${target} PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO )
endfunction()

# Targets:

enable_testing()

foreach( target ${BASENAMES} )
    make_target( ${target} )
endforeach()

# end of file
