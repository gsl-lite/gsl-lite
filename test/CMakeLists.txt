# Copyright 2015-2017 by Martin Moene
#
# gsl-lite is based on GSL: Guideline Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT).

cmake_minimum_required( VERSION 2.8.12 )

project( test )

include_directories  ( "${PROJECT_SOURCE_DIR}/../include/gsl" )

set( SOURCES gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp )

add_definitions( -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS )

set( HAS_STD_FLAGS  FALSE )
set( HAS_CPP11_FLAG FALSE )
set( HAS_CPP14_FLAG FALSE )
set( HAS_CPP17_FLAG FALSE )

if( MSVC )
    add_compile_options( -W3 -EHsc )
    add_definitions( -D_SCL_SECURE_NO_WARNINGS -DNOMINMAX )

elseif( CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang" )

    set( HAS_STD_FLAGS TRUE )

    # GNU: available -std flags depends on version
    if( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8.0 )
            set( HAS_CPP11_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9.2 )
            set( HAS_CPP14_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.1.0 )
            set( HAS_CPP17_FLAG TRUE )
        endif()
    endif()

    # Clang: available -std flags depends on version
    if( CMAKE_CXX_COMPILER_ID MATCHES Clang )
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3.0 )
            set( HAS_CPP11_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.4.0 )
            set( HAS_CPP14_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0.0 )
            set( HAS_CPP17_FLAG TRUE )
        endif()
    endif()

    add_executable( gsl-lite-cpp98.t ${SOURCES} )
    target_compile_options( gsl-lite-cpp98.t  PUBLIC -std=c++98 )

    add_executable( gsl-lite-cpp03.t ${SOURCES} )
    target_compile_options( gsl-lite-cpp03.t  PUBLIC -std=c++03 )

    if( HAS_CPP11_FLAG )
        add_executable( gsl-lite-cpp11.t ${SOURCES} )
        target_compile_options( gsl-lite-cpp11.t  PUBLIC -std=c++11 )
    endif()

    if( HAS_CPP14_FLAG )
        add_executable( gsl-lite-cpp14.t ${SOURCES} )
        target_compile_options( gsl-lite-cpp14.t  PUBLIC -std=c++14 )
    endif()

    if( HAS_CPP17_FLAG )
        add_executable( gsl-lite-cpp17.t ${SOURCES} )
        target_compile_options( gsl-lite-cpp17.t  PUBLIC -std=c++17 )
    endif()

    add_compile_options( -Wall -Wno-missing-braces -fno-elide-constructors -Wconversion -Wsign-conversion -Wno-string-conversion )

elseif( CMAKE_CXX_COMPILER_ID MATCHES "Intel" )
# as is
else()
# as is
endif()

if( NOT HAS_STD_FLAGS )
    add_executable( gsl-lite.t ${SOURCES} )
    target_link_libraries( gsl-lite.t gsl_lite )
endif()

# configure unit tests via CTest:

enable_testing()

if( NOT HAS_STD_FLAGS )
    add_test(     NAME test          COMMAND gsl-lite.t --pass )
    add_test(     NAME list_version  COMMAND gsl-lite.t --version )
    add_test(     NAME list_tags     COMMAND gsl-lite.t --list-tags )
    add_test(     NAME list_tests    COMMAND gsl-lite.t --list-tests )
else()
    add_test(     NAME test-cpp98    COMMAND gsl-lite-cpp98.t )
    add_test(     NAME test-cpp03    COMMAND gsl-lite-cpp03.t )
    if( HAS_CPP11_FLAG )
        add_test( NAME test-cpp11    COMMAND gsl-lite-cpp11.t )
    endif()
    if( HAS_CPP14_FLAG )
        add_test( NAME test-cpp14    COMMAND gsl-lite-cpp14.t --pass )
    endif()
    if( HAS_CPP17_FLAG )
        add_test( NAME test-cpp17    COMMAND gsl-lite-cpp17.t --pass )
    endif()
endif()

# end of file
