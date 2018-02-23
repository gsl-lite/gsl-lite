# Copyright 2015-2018 by Martin Moene
#
# gsl-lite is based on GSL: Guideline Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT).

cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

project( gsl-lite-example )

set( INCDIRS "${PROJECT_SOURCE_DIR}/../include/gsl" )
set( OPTIONS "" )
set( DEFINITIONS "" )

set( SOURCES 
    01-basic.cpp 
    02-span.cpp 
)

# Make target, compile for given standard if specified:

function( make_target source )
    string( REPLACE ".cpp" "" target "${source}" )
    add_executable            ( ${target} ${source}  )
    target_compile_options    ( ${target} PRIVATE ${OPTIONS} )
    target_compile_definitions( ${target} PRIVATE ${DEFINITIONS} )
    target_include_directories( ${target} PRIVATE ${INCDIRS} )
endfunction()

string( REPLACE ".cpp" "" BASENAMES "${SOURCES}" )

foreach( target ${BASENAMES} )
    make_target( ${target} )
endforeach()

# end of file
