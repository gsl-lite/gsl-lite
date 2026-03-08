# Copyright 2015-2019 by Martin Moene
# Copyright 2019-2025 by Moritz Beutel
#
# https://github.com/gsl-lite/gsl-lite
#
# This code is licensed under the MIT License (MIT).


cmake_minimum_required( VERSION 3.16 FATAL_ERROR )

project( gsl-lite-test LANGUAGES CXX )

message( STATUS "Subproject '${PROJECT_NAME}', programs 'gsl-lite-*'")

include(${CMAKE_CURRENT_SOURCE_DIR}/MakeTestTarget.cmake)


function( make_test_targets nameRoot minStd maxStd )
    if( NOT HAS_STD_FLAGS )
        make_test_target( ${nameRoot}.t ${ARGN} )
    else()
        # MSVC 2015 and later support only /std:c++14 and newer
        if( NOT MSVC AND NOT 3 LESS minStd )
            if( HAS_CPP98_FLAG )
                make_test_target( ${nameRoot}-cpp98.t STD 98 ${ARGN} )
            else()
                make_test_target( ${nameRoot}-cpp98.t ${ARGN} )
            endif()
        endif()
        if( NOT MSVC AND HAS_CPP11_FLAG AND NOT 11 LESS minStd AND NOT maxStd LESS 11 )
            make_test_target( ${nameRoot}-cpp11.t STD 11 ${ARGN} )
        endif()
        if( HAS_CPP14_FLAG AND NOT 14 LESS minStd AND NOT maxStd LESS 14 )
            make_test_target( ${nameRoot}-cpp14.t STD 14 ${ARGN} )
        endif()
        if( HAS_CPP17_FLAG AND NOT 17 LESS minStd AND NOT maxStd LESS 17 )
            make_test_target( ${nameRoot}-cpp17.t STD 17 ${ARGN} )
            enable_msvs_guideline_checker( ${nameRoot}-cpp17.t )
        endif()
        if( HAS_CPP20_FLAG AND NOT 20 LESS minStd AND NOT maxStd LESS 20 )
            make_test_target( ${nameRoot}-cpp20.t STD 20 ${ARGN} )
            enable_msvs_guideline_checker( ${nameRoot}-cpp20.t )
        endif()
        if( HAS_CPP23_FLAG AND NOT 23 LESS minStd AND NOT maxStd LESS 23 )
            make_test_target( ${nameRoot}-cpp23.t STD 23 ${ARGN} )
            enable_msvs_guideline_checker( ${nameRoot}-cpp23.t )
        endif()
        if( HAS_CPPLATEST_FLAG AND maxStd GREATER 20 )
            make_test_target( ${nameRoot}-cpplatest.t STD latest ${ARGN} )
        endif()
    endif()
endfunction()


# Add test targets for version-0 and version-1 defaults:

set( SOURCES
    "gsl-lite.t.cpp"
    "assert.t.cpp"
    "at.t.cpp"
    "byte.t.cpp"
    "emulation.t.cpp"
    "issue.t.cpp"
    "not_null.t.cpp"
    "owner.t.cpp"
    "span.t.cpp"
    "string_span.t.cpp"
    "util.t.cpp"
)
make_test_targets(
    "gsl-lite-v0" 3 99
    SOURCES ${SOURCES}
    DEFAULTS_VERSION v0
    EXTRA_OPTIONS
        "-Dgsl_CONFIG_ACKNOWLEDGE_NONSTANDARD_ABI=1"
)
make_test_targets(
    "gsl-lite-v1" 3 99
    SOURCES ${SOURCES}
    DEFAULTS_VERSION v1
)


# Add test targets for no-exception build:

make_test_targets(
    "gsl-lite-v1-no-exc" 3 99
    SOURCES ${SOURCES}
    DEFAULTS_VERSION v1
    CONTRACT_VIOLATION TERMINATES
    NO_EXCEPTIONS
    COMPILE_ONLY
)


# Add test targets for special configurations:

make_test_targets( "gsl-lite-v1-special" 3 14
    SOURCES
        gsl-lite.t.cpp
        not_null.t.cpp
        util.t.cpp
    EXTRA_OPTIONS
        "-Dgsl_FEATURE_EXPERIMENTAL_RETURN_GUARD=1"
        "-Dgsl_CONFIG_SPAN_INDEX_TYPE=std::ptrdiff_t"
        "-Dgsl_CONFIG_ACKNOWLEDGE_NONSTANDARD_ABI=1"
        "-Dgsl_CONFIG_TRANSPARENT_NOT_NULL=0"
        "-DNOT_NULL_GET_BY_CONST_REF=1"
        "-Dgsl_FEATURE_SPAN=0"
    DEFAULTS_VERSION v1
)

make_test_targets( "gsl-lite-v1-unenforced" 3 20
    SOURCES
        gsl-lite.t.cpp
        assert.t.cpp
    DEFAULTS_VERSION v1
    CONTRACT_CHECKING OFF
    UNENFORCED_CONTRACTS ELIDE
    COMPILE_ONLY
)

if( ( MSVC AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.0 ) # VS 2015 and later
        OR CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang" )
    make_test_targets( "gsl-lite-v1-assume" 3 20
        SOURCES
            gsl-lite.t.cpp
            assert.t.cpp
        DEFAULTS_VERSION v1
        CONTRACT_CHECKING OFF
        UNENFORCED_CONTRACTS ASSUME
        COMPILE_ONLY
    )
endif()

if( ( MSVC AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.30 ) # VS 2022 and later
        OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14 AND NOT APPLE )
    make_test_targets( "gsl-lite-v1-stacktrace" 23 23
        SOURCES
            gsl-lite.t.cpp
            assert.t.cpp
        EXTRA_OPTIONS "$<$<CXX_COMPILER_ID:GNU>:-lstdc++exp>"
        DEFAULTS_VERSION v1
        CONTRACT_CHECKING ON
        CONTRACT_VIOLATION TERMINATES_WITH_STACKTRACE
        UNENFORCED_CONTRACTS ELIDE
        COMPILE_ONLY
    )
endif()

make_test_targets( "gsl-lite-v1-terminates" 3 20
    SOURCES
        gsl-lite.t.cpp
        assert.t.cpp
    DEFAULTS_VERSION v1
    CONTRACT_VIOLATION TERMINATES
    COMPILE_ONLY
)

make_test_targets( "gsl-lite-v1-asserts" 3 20
    SOURCES
        gsl-lite.t.cpp
        assert.t.cpp
    DEFAULTS_VERSION v1
    CONTRACT_VIOLATION ASSERTS
    COMPILE_ONLY
)

make_test_targets( "gsl-lite-v1-calls-handler" 3 20
    SOURCES
        gsl-lite.t.cpp
        assert.t.cpp
    DEFAULTS_VERSION v1
    CONTRACT_VIOLATION CALLS_HANDLER
    COMPILE_ONLY
)

if( ( MSVC AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 17.0 ) # VS 2012 and later
        OR CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang" )
    make_test_targets( "gsl-lite-v1-traps" 3 20
        SOURCES
            gsl-lite.t.cpp
            assert.t.cpp
        DEFAULTS_VERSION v1
        CONTRACT_VIOLATION TRAPS
        COMPILE_ONLY
    )
endif()

if( GSL_LITE_OPT_BUILD_STATIC_ANALYSIS_DEMOS AND MSVC AND CMAKE_GENERATOR MATCHES "Visual Studio" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.2 )
    make_test_target( "gsl-lite-v1-cpplatest-CppCoreCheck"
        STD latest
        EXTRA_OPTIONS
            "-Dgsl_FEATURE_GSL_COMPATIBILITY_MODE=1"
        SOURCES
            "core_check.t.cpp"
            "core_check_lite.t.cpp"
        DEFAULTS_VERSION v1
        COMPILE_ONLY
        NO_PCH
    )
    enable_msvs_guideline_checker( "gsl-lite-v1-cpplatest-CppCoreCheck" )
endif()
