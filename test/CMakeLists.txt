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


function(make_test_targets nameRoot srcs maxStd extraOptions defaultsVersion )
    if( NOT HAS_STD_FLAGS )
        make_test_target( ${nameRoot}.t "${srcs}" "" "${extraOptions}" ${defaultsVersion} )
    else()
        if( MSVC )
            # MSVC 2015 and later support /std:c++14 but ignore it because they are in C++14 mode by default, so we won't bother.
            make_test_target( ${nameRoot}-cpp14.t "${srcs}" "" "${extraOptions}" ${defaultsVersion} )
        else()
            if( HAS_CPP98_FLAG )
                make_test_target( ${nameRoot}-cpp98.t "${srcs}" 98 "${extraOptions}" ${defaultsVersion} )
            else()
                make_test_target( ${nameRoot}-cpp98.t "${srcs}" "" "${extraOptions}" ${defaultsVersion} )
            endif()

            if( HAS_CPP11_FLAG AND NOT maxStd LESS 11 )
                make_test_target( ${nameRoot}-cpp11.t "${srcs}" 11 "${extraOptions}" ${defaultsVersion} )
            endif()

            if( HAS_CPP14_FLAG AND NOT maxStd LESS 14 )
                make_test_target( ${nameRoot}-cpp14.t "${srcs}" 14 "${extraOptions}" ${defaultsVersion} )
            endif()
        endif()

        if( HAS_CPP17_FLAG AND NOT maxStd LESS 17 )
            make_test_target( ${nameRoot}-cpp17.t "${srcs}" 17 "${extraOptions}" ${defaultsVersion} )
            enable_msvs_guideline_checker( ${nameRoot}-cpp17.t )
        endif()

        if( HAS_CPP20_FLAG AND NOT maxStd LESS 20 )
            make_test_target( ${nameRoot}-cpp20.t "${srcs}" 20 "${extraOptions}" ${defaultsVersion} )
            enable_msvs_guideline_checker( ${nameRoot}-cpp20.t )
        endif()

        if( HAS_CPPLATEST_FLAG AND maxStd GREATER 20 )
            make_test_target( ${nameRoot}-cpplatest.t "${srcs}" latest "${extraOptions}" ${defaultsVersion} )
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
make_test_targets( "${PROGRAM}-v0" "${SOURCES}" 99 "" v0 )
make_test_targets( "${PROGRAM}-v1" "${SOURCES}" 99 "" v1 )


# Add test targets for special configurations:

set( SPECIAL_SOURCES
    gsl-lite.t.cpp
    not_null.t.cpp
)
set( SPECIAL_OPTIONS
    -Dgsl_FEATURE_EXPERIMENTAL_RETURN_GUARD=1
    -Dgsl_CONFIG_SPAN_INDEX_TYPE=std::ptrdiff_t
    -Dgsl_CONFIG_NOT_NULL_DIRECT_CTOR=0
)
make_test_targets( "${PROGRAM}-v1-special" "${SPECIAL_SOURCES}" 11 "${SPECIAL_OPTIONS}" v1 )


# end of file
