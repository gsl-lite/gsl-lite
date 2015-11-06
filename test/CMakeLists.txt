# Copyright 2015 by Martin Moene
#
# gsl-lite is based on GSL: Guidelines Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT). 

cmake_minimum_required( VERSION 2.8 )

project( test )

include_directories  ( "${PROJECT_SOURCE_DIR}/../include/gsl" )

set( SOURCES gsl-lite.t.cpp assert.t.cpp at.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp )

add_executable (   gsl-lite.t ${SOURCES} )
add_definitions( -Dgsl_CONFIG_THROWS_FOR_TESTING )

set( SINGLE_TEST_PROGRAM TRUE )

# GNU: have -std=c++14 and lower?
if( ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU" )
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)

    # command line option -std=c++14 since gcc 4.9.2:
    if( GCC_VERSION VERSION_GREATER 4.9.2 OR GCC_VERSION VERSION_EQUAL 4.9.2 )
        set( SINGLE_TEST_PROGRAM FALSE )
    endif()
endif()

if( ${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC" )
    add_compile_options( -W3 -EHsc )

elseif( ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU" OR
        ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" )
    add_executable( gsl-lite-cpp98.t ${SOURCES} )
    add_executable( gsl-lite-cpp03.t ${SOURCES} )
    add_executable( gsl-lite-cpp11.t ${SOURCES} )
    add_executable( gsl-lite-cpp14.t ${SOURCES} )

    add_compile_options( -Wall -Wno-missing-braces )

    target_compile_options( gsl-lite-cpp98.t  PUBLIC -std=c++98 )
    target_compile_options( gsl-lite-cpp03.t  PUBLIC -std=c++03 )
    target_compile_options( gsl-lite-cpp11.t  PUBLIC -std=c++11 )
    target_compile_options( gsl-lite-cpp14.t  PUBLIC -std=c++14 )

elseif( ${CMAKE_CXX_COMPILER_ID} MATCHES "Intel" )
# as is
else()
# as is
endif()

# configure unit tests via CTest:

enable_testing()

 if( ${SINGLE_TEST_PROGRAM} )
add_test( NAME test          COMMAND gsl-lite.t --pass )
 else()
add_test( NAME test-cpp98    COMMAND gsl-lite-cpp98.t )
add_test( NAME test-cpp03    COMMAND gsl-lite-cpp03.t )
add_test( NAME test-cpp11    COMMAND gsl-lite-cpp11.t )
add_test( NAME test-cpp14    COMMAND gsl-lite-cpp14.t --pass )
 endif()
add_test( NAME list_version  COMMAND gsl-lite.t --version )
add_test( NAME list_tags     COMMAND gsl-lite.t --list-tags )
add_test( NAME list_tests    COMMAND gsl-lite.t --list-tests )

# end of file
