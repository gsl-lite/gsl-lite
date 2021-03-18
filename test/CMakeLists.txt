# Copyright 2015-2018 by Martin Moene
#
# gsl-lite is based on GSL: Guidelines Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT).

cmake_minimum_required( VERSION 3.5 FATAL_ERROR )

project( gsl-lite-test LANGUAGES CXX )

set( unit_name "gsl" )
set( PACKAGE   ${unit_name}-lite )
set( PROGRAM   ${unit_name}-lite )

message( STATUS "Subproject '${PROJECT_NAME}', programs '${PROGRAM}-*'")

include(${CMAKE_CURRENT_SOURCE_DIR}/MakeTestTarget.cmake)


function( make_test_targets nameRoot maxStd )
    if( NOT HAS_STD_FLAGS )
        make_test_target( ${nameRoot}.t ${ARGN} )
    else()
        if( MSVC )
            # MSVC 2015 and later support /std:c++14 but ignore it because they are in C++14 mode by default, so we won't bother.
            make_test_target( ${nameRoot}-cpp14.t ${ARGN} )
        else()
            if( HAS_CPP98_FLAG )
                make_test_target( ${nameRoot}-cpp98.t STD 98 ${ARGN} )
            else()
                make_test_target( ${nameRoot}-cpp98.t ${ARGN} )
            endif()
            if( HAS_CPP11_FLAG AND NOT maxStd LESS 11 )
                make_test_target( ${nameRoot}-cpp11.t STD 11 ${ARGN} )
            endif()
            if( HAS_CPP14_FLAG AND NOT maxStd LESS 14 )
                make_test_target( ${nameRoot}-cpp14.t STD 14 ${ARGN} )
            endif()
        endif()
        if( HAS_CPP17_FLAG AND NOT maxStd LESS 17 )
            make_test_target( ${nameRoot}-cpp17.t STD 17 ${ARGN} )
            enable_msvs_guideline_checker( ${nameRoot}-cpp17.t )
        endif()
        if( HAS_CPP20_FLAG AND NOT maxStd LESS 20 )
            make_test_target( ${nameRoot}-cpp20.t STD 20 ${ARGN} )
            enable_msvs_guideline_checker( ${nameRoot}-cpp20.t )
        endif()
        if( HAS_CPPLATEST_FLAG AND maxStd GREATER 20 )
            make_test_target( ${nameRoot}-cpplatest.t STD latest ${ARGN} )
        endif()
    endif()
endfunction()


# Add test targets for version-0 and version-1 defaults:

set( SOURCES
    gsl-lite.t.cpp
    assert.t.cpp
    at.t.cpp
    byte.t.cpp
    emulation.t.cpp
    issue.t.cpp
    not_null.t.cpp
    owner.t.cpp
    span.t.cpp
    string_span.t.cpp
    util.t.cpp
)
make_test_targets( "${PROGRAM}-v0" 99 SOURCES ${SOURCES} DEFAULTS_VERSION v0 )
make_test_targets( "${PROGRAM}-v1" 99 SOURCES ${SOURCES} DEFAULTS_VERSION v1 )


# Add test targets for special configurations:

set( SPECIAL_SOURCES
    gsl-lite.t.cpp
    not_null.t.cpp
)
set( SPECIAL_OPTIONS
    "-Dgsl_FEATURE_EXPERIMENTAL_RETURN_GUARD=1"
    "-Dgsl_CONFIG_SPAN_INDEX_TYPE=std::ptrdiff_t"
)
make_test_targets( "${PROGRAM}-v1-special" 11 SOURCES ${SPECIAL_SOURCES} EXTRA_OPTIONS ${SPECIAL_OPTIONS} DEFAULTS_VERSION v1 )


# end of file
