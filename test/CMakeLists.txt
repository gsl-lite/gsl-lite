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

# Test sources:

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
# Configure gsl-lite for testing:

set( GSL_CONFIG
    -Dgsl_TESTING_
    -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS
    -Dgsl_CONFIG_CONTRACT_CHECKING_AUDIT
)
# Preset available C++ language compiler flags:

set( HAS_STD_FLAGS  FALSE )
set( HAS_CPP98_FLAG FALSE )
set( HAS_CPP11_FLAG FALSE )
set( HAS_CPP14_FLAG FALSE )
set( HAS_CPP17_FLAG FALSE )
set( HAS_CPP20_FLAG FALSE )
set( HAS_CPPLATEST_FLAG FALSE )

# Determine compiler-specifics for MSVC, GNUC, Clang:

if( MSVC )
    message( STATUS "Matched: MSVC")

    set( HAS_STD_FLAGS TRUE )

    set( OPTIONS     /EHsc /WX /W4 )
    set( DEFINITIONS -D_SCL_SECURE_NO_WARNINGS )

    if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.00 )
        set( HAS_CPP14_FLAG TRUE )
        set( HAS_CPPLATEST_FLAG TRUE )
    endif()
    if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.11 )
        set( HAS_CPP17_FLAG TRUE )
    endif()

elseif( CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang" )
    message( STATUS "CompilerId: '${CMAKE_CXX_COMPILER_ID}'")

    set( HAS_STD_FLAGS  TRUE )
    set( HAS_CPP98_FLAG TRUE )

    set( OPTIONS
        -Werror
        -Wall
        -Wno-missing-braces
        -Wconversion
        -Wsign-conversion
        -fno-elide-constructors
        -fstrict-aliasing -Wstrict-aliasing=2
    )
    set( DEFINITIONS "" )

    # GNU: available -std flags depends on version
    if( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
        message( STATUS "Matched: GNU")

        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8.0 )
            set( HAS_CPP11_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9.2 )
            set( HAS_CPP14_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.1.0 )
            set( HAS_CPP17_FLAG TRUE )
        endif()

    # AppleClang: available -std flags depends on version
    elseif( CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" )
        message( STATUS "Matched: AppleClang")

        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0.0 )
            set( HAS_CPP11_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.1.0 )
            set( HAS_CPP14_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.2.0 )
            set( HAS_CPP17_FLAG TRUE )
        endif()

    # Clang: available -std flags depends on version
    elseif( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
        message( STATUS "Matched: Clang")

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

elseif( CMAKE_CXX_COMPILER_ID MATCHES "Intel" )
    # as is
    message( STATUS "Matched: Intel")
else()
    # as is
    message( STATUS "Matched: nothing")
endif()

# Enable MS C++ Core Guidelines checker if MSVC (but only when using MSBuild because Ninja cannot find the <CppCoreCheck/warnings.h> header):

function( enable_msvs_guideline_checker target )
    if( MSVC AND CMAKE_GENERATOR MATCHES "Visual Studio" )
        target_compile_definitions( ${target} PRIVATE gsl_TESTING_CPPCORECHECK_ )
        set_target_properties( ${target} PROPERTIES
            VS_GLOBAL_EnableCppCoreCheck true
            VS_GLOBAL_CodeAnalysisRuleSet CppCoreCheckRules.ruleset
            VS_GLOBAL_RunCodeAnalysis true )
    endif()
endfunction()

# Make target, compile for given standard if specified:

function( make_test_target target std sources defaults_version )
    message( STATUS "Make target: '${std}-${defaults_version}'" )

    add_executable            ( ${target} ${sources}  )
    target_link_libraries     ( ${target} PRIVATE ${PACKAGE}-${defaults_version} )
    target_compile_options    ( ${target} PRIVATE ${OPTIONS} )
    target_compile_definitions( ${target} PRIVATE ${DEFINITIONS} ${GSL_CONFIG} )

    if( NOT CMAKE_VERSION VERSION_LESS 3.16 # VERSION_GREATER_EQUAL doesn't exist in CMake 3.5
            AND NOT ( CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_SYSTEM_NAME MATCHES "Darwin" ) ) # and GCC on MacOS has trouble with addresses of some text segments in the PCH
        target_precompile_headers( ${target} PRIVATE gsl-lite.t.hpp )
    endif()

    if( std )
        if( MSVC )
            target_compile_options( ${target} PRIVATE -std:c++${std} )
        else()
            # Necessary for clang 3.x:
            target_compile_options( ${target} PRIVATE -std=c++${std} )
            # Ok for clang 4 and later:
            # set( CMAKE_CXX_STANDARD ${std} )
            # set( CMAKE_CXX_STANDARD_REQUIRED ON )
            # set( CMAKE_CXX_EXTENSIONS OFF )
        endif()
    endif()

    add_test(     NAME ${target}             COMMAND ${target} )
    set_property( TEST ${target} PROPERTY FAIL_REGULAR_EXPRESSION "Sanitizer" )

endfunction()

# Add targets and configure unit tests via CTest:

enable_testing()


if( NOT HAS_STD_FLAGS )
    make_test_target( ${PROGRAM}.t        "" "${SOURCES}" v0 )
    make_test_target( ${PROGRAM}-altcfg.t "" "${SOURCES}" v1 )
else()
    if( MSVC )
        # MSVC 2015 and later support /std:c++14 but ignore it because they are in C++14 mode by default, so we won't bother.
        make_test_target( ${PROGRAM}-base.t        "" "${SOURCES}" v0 )
        make_test_target( ${PROGRAM}-altcfg-base.t "" "${SOURCES}" v1 )
    else()
        if( HAS_CPP98_FLAG )
            make_test_target( ${PROGRAM}-cpp98.t        98 "${SOURCES}" v0 )
            make_test_target( ${PROGRAM}-altcfg-cpp98.t 98 "${SOURCES}" v1 )
        else()
            make_test_target( ${PROGRAM}-cpp98.t        "" "${SOURCES}" v0 )
            make_test_target( ${PROGRAM}-altcfg-cpp98.t "" "${SOURCES}" v1 )
        endif()

        if( HAS_CPP11_FLAG )
            make_test_target( ${PROGRAM}-cpp11.t        11 "${SOURCES}" v0 )
            make_test_target( ${PROGRAM}-altcfg-cpp11.t 11 "${SOURCES}" v1 )
        endif()

        if( HAS_CPP14_FLAG )
            make_test_target( ${PROGRAM}-cpp14.t        14 "${SOURCES}" v0 )
            make_test_target( ${PROGRAM}-altcfg-cpp14.t 14 "${SOURCES}" v1 )
        endif()
    endif()

    if( HAS_CPP17_FLAG )
        make_test_target( ${PROGRAM}-cpp17.t            17 "${SOURCES}" v0 )
        enable_msvs_guideline_checker( ${PROGRAM}-cpp17.t )
        make_test_target( ${PROGRAM}-altcfg-cpp17.t     17 "${SOURCES}" v1 )
        enable_msvs_guideline_checker( ${PROGRAM}-altcfg-cpp17.t )
    endif()

    if( HAS_CPPLATEST_FLAG )
        make_test_target( ${PROGRAM}-cpplatest.t        latest "${SOURCES}" v0 )
        make_test_target( ${PROGRAM}-altcfg-cpplatest.t latest "${SOURCES}" v1 )
    endif()
endif()

# end of file
