# Copyright 2015-2018 by Martin Moene
#
# gsl-lite is based on GSL: Guideline Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT).

cmake_minimum_required( VERSION 3.0 )

project( test )

function( enable_msvs_guideline_checker target )
    set_target_properties( ${target} PROPERTIES
        VS_GLOBAL_EnableCppCoreCheck true
        VS_GLOBAL_CodeAnalysisRuleSet CppCoreCheckRules.ruleset
        VS_GLOBAL_RunCodeAnalysis true )
endfunction()

include_directories( "${PROJECT_SOURCE_DIR}/../include/gsl" )

set( SOURCES gsl-lite.t.cpp assert.t.cpp at.t.cpp byte.t.cpp issue.t.cpp not_null.t.cpp owner.t.cpp span.t.cpp string_span.t.cpp util.t.cpp )

set( GSL_CONFIG -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS )

set( HAS_STD_FLAGS  FALSE )
set( HAS_CPP98_FLAG FALSE )
set( HAS_CPP03_FLAG FALSE )
set( HAS_CPP11_FLAG FALSE )
set( HAS_CPP14_FLAG FALSE )
set( HAS_CPP17_FLAG FALSE )
set( HAS_CPP20_FLAG FALSE )

if( MSVC )
    set( HAS_STD_FLAGS TRUE )

    set( STD_OPT     -std: )
    set( OPTIONS     -W3 -EHsc )
    set( DEFINITIONS -D_SCL_SECURE_NO_WARNINGS -DNOMINMAX ${GSL_CONFIG} )

    if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.00 )
        set( HAS_CPP14_FLAG TRUE )
        set( HAS_CPPLATEST_FLAG TRUE )
    endif()
    if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.11 )
        set( HAS_CPP17_FLAG TRUE )
    endif()

elseif( CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang" )

    set( HAS_STD_FLAGS  TRUE )
    set( HAS_CPP98_FLAG TRUE )
    set( HAS_CPP03_FLAG TRUE )

    set( STD_OPT     -std= )
    set( OPTIONS     -Wall -Wno-missing-braces -fno-elide-constructors -Wconversion -Wsign-conversion -Wno-string-conversion ${GSL_CONFIG} )
    set( DEFINITIONS "" )

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

elseif( CMAKE_CXX_COMPILER_ID MATCHES "Intel" )
# as is
else()
# as is
endif()

# add generic executable, unless -std flags can be specified:

if( NOT HAS_STD_FLAGS )
    add_executable( gsl-lite.t ${SOURCES} )
    target_link_libraries( gsl-lite.t gsl-lite )
else()
    # unconditionally add C++98 variant for MSVC:
    add_executable                ( gsl-lite-cpp98.t ${SOURCES} )
    target_compile_definitions    ( gsl-lite-cpp98.t PRIVATE ${DEFINITIONS} )
    target_compile_options        ( gsl-lite-cpp98.t PRIVATE ${OPTIONS} )
    if( HAS_CPP98_FLAG )
        target_compile_options    ( gsl-lite-cpp98.t PRIVATE ${STD_OPT}c++98 )
    endif()

    if( HAS_CPP03_FLAG )
        add_executable            ( gsl-lite-cpp03.t ${SOURCES} )
        target_compile_options    ( gsl-lite-cpp03.t PRIVATE ${OPTIONS} ${STD_OPT}c++03 )
        target_compile_definitions( gsl-lite-cpp03.t PRIVATE ${DEFINITIONS} )
    endif()

    if( HAS_CPP11_FLAG )
        add_executable            ( gsl-lite-cpp11.t ${SOURCES} )
        target_compile_options    ( gsl-lite-cpp11.t PRIVATE ${OPTIONS} ${STD_OPT}c++11 )
        target_compile_definitions( gsl-lite-cpp11.t PRIVATE ${DEFINITIONS} )
    endif()

    if( HAS_CPP14_FLAG )
        add_executable            ( gsl-lite-cpp14.t ${SOURCES} )
        target_compile_options    ( gsl-lite-cpp14.t PRIVATE ${OPTIONS} ${STD_OPT}c++14 )
        target_compile_definitions( gsl-lite-cpp14.t PRIVATE ${DEFINITIONS} )
    endif()

    if( HAS_CPP17_FLAG )
        add_executable            ( gsl-lite-cpp17.t ${SOURCES} )
        target_compile_options    ( gsl-lite-cpp17.t PRIVATE ${OPTIONS} ${STD_OPT}c++17 )
        target_compile_definitions( gsl-lite-cpp17.t PRIVATE ${DEFINITIONS} )
    endif()

    if( HAS_CPPLATEST_FLAG )
        add_executable            ( gsl-lite-cpplatest.t ${SOURCES} )
        target_compile_options    ( gsl-lite-cpplatest.t PRIVATE ${OPTIONS} ${STD_OPT}c++latest )
        target_compile_definitions( gsl-lite-cpplatest.t PRIVATE ${DEFINITIONS} )
    endif()
endif()

# configure unit tests via CTest:

enable_testing()

if( HAS_STD_FLAGS )
    # unconditionally add C++98 variant for MSVC:
    add_test(     NAME test-cpp98     COMMAND gsl-lite-cpp98.t )

    if( HAS_CPP03_FLAG )
        add_test( NAME test-cpp03     COMMAND gsl-lite-cpp03.t )
    endif()
    if( HAS_CPP11_FLAG )
        add_test( NAME test-cpp11     COMMAND gsl-lite-cpp11.t )
    endif()
    if( HAS_CPP14_FLAG )
        add_test( NAME test-cpp14     COMMAND gsl-lite-cpp14.t )
    endif()
    if( HAS_CPP17_FLAG )
        add_test( NAME test-cpp17     COMMAND gsl-lite-cpp17.t )
    endif()
    if( HAS_CPPLATEST_FLAG )
        add_test( NAME test-cpplatest COMMAND gsl-lite-cpplatest.t )
    endif()
else()
    add_test(     NAME test           COMMAND gsl-lite.t --pass )
    add_test(     NAME list_version   COMMAND gsl-lite.t --version )
    add_test(     NAME list_tags      COMMAND gsl-lite.t --list-tags )
    add_test(     NAME list_tests     COMMAND gsl-lite.t --list-tests )
endif()

# end of file
