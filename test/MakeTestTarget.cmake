# Copyright 2015-2018 by Martin Moene
#
# gsl-lite is based on GSL: Guidelines Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT).


if( DEFINED _makeTestTarget )
    return()  # prevent multiple inclusion
endif()
set( _makeTestTarget )



# Configure gsl-lite for testing:

set( GSL_CONFIG
    "gsl_TESTING_"
    "gsl_CONFIG_CONTRACT_CHECKING_AUDIT"
)

set( CUDA_OPTIONS )

# Preset available C++ language compiler flags:

set( HAS_STD_FLAGS  FALSE )
set( HAS_CPP98_FLAG FALSE )
set( HAS_CPP11_FLAG FALSE )
set( HAS_CPP14_FLAG FALSE )
set( HAS_CPP17_FLAG FALSE )
set( HAS_CPP20_FLAG FALSE )
set( HAS_CPPLATEST_FLAG FALSE )

# Preset available CUDA language compiler flags:

set( HAS_CUDA14_FLAG FALSE )
set( HAS_CUDA17_FLAG FALSE )

set( HOST_COMPILER_PREFIX "" )

# Determine compiler-specifics for MSVC, GNUC, Clang:

if( MSVC )
    set( HAS_STD_FLAGS TRUE )

    # remove "/EHx" from CMAKE_CXX_FLAGS if present
    if( CMAKE_CXX_FLAGS MATCHES "/EH[ascr-]+" )
        string( REGEX REPLACE "/EH[ascr-]+" " " CMAKE_CXX_FLAGS_NEW "${CMAKE_CXX_FLAGS}" )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_NEW}" )
    endif()
    
    # remove "/Wx" from CMAKE_CXX_FLAGS if present
    if( CMAKE_CXX_FLAGS MATCHES "/W[0-4]" )
        string( REGEX REPLACE "/W[0-4]" " " CMAKE_CXX_FLAGS_NEW "${CMAKE_CXX_FLAGS}" )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_NEW}" )
    endif()

    # clang-cl: available std flags depends on version
    if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
        message( STATUS "Matched: clang-cl ${CMAKE_CXX_COMPILER_VERSION}" )

        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3.0 )
            set( HAS_CPP11_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.4.0 )
            set( HAS_CPP14_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0.0 )
            set( HAS_CPP17_FLAG TRUE )
        endif()
    else()
        message( STATUS "Matched: MSVC ${CMAKE_CXX_COMPILER_VERSION}" )
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.00 )
            set( HAS_CPP14_FLAG TRUE )
            set( HAS_CPPLATEST_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.11 )
            set( HAS_CPP17_FLAG TRUE )
        endif()
    endif()

elseif( CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang" )

    set( HAS_STD_FLAGS  TRUE )
    set( HAS_CPP98_FLAG TRUE )

    # GNU: available -std flags depends on version
    if( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
        message( STATUS "Matched: GNU ${CMAKE_CXX_COMPILER_VERSION}" )

        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.7.0 )
            set( HAS_CPP11_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9.2 )
            set( HAS_CPP14_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.1.0 )
            set( HAS_CPP17_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0.0 )
            set( HAS_CPP20_FLAG TRUE )
        endif()

    # AppleClang: available -std flags depends on version
    elseif( CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" )
        message( STATUS "Matched: AppleClang ${CMAKE_CXX_COMPILER_VERSION}" )

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
        message( STATUS "Matched: Clang ${CMAKE_CXX_COMPILER_VERSION}" )

        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3.0 )
            set( HAS_CPP11_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.4.0 )
            set( HAS_CPP14_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0.0 )
            set( HAS_CPP17_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0.0 )
            set( HAS_CPP20_FLAG TRUE )
        endif()
    endif()

elseif( CMAKE_CXX_COMPILER_ID MATCHES "Intel" )
    message( STATUS "Matched: Intel ${CMAKE_CXX_COMPILER_VERSION}" )
else()
    message( STATUS "Matched: nothing (CompilerId: '${CMAKE_CXX_COMPILER_ID}')")
endif()

# Determine compiler-specifics for NVCC:

get_property( _languages GLOBAL PROPERTY ENABLED_LANGUAGES )
if( CUDA IN_LIST _languages )
    if( CMAKE_CUDA_COMPILER_ID MATCHES "NVIDIA" )
        message( STATUS "Matched: NVCC ${CMAKE_CUDA_COMPILER_VERSION}" )

        # Set NVCC-specific options:
        set( HOST_COMPILER_PREFIX "-Xcompiler=" )
        list( APPEND CUDA_OPTIONS "--Werror" "all-warnings" "-G" )

        set( HAS_CUDA14_FLAG TRUE )
        if( CMAKE_CUDA_COMPILER_VERSION VERSION_GREATER_EQUAL 11.0 )
            set( HAS_CUDA17_FLAG TRUE )
        endif()
    endif()
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

function( make_test_target target )

    set( optionArgs CUDA NO_EXCEPTIONS COMPILE_ONLY NO_PCH )
    set( oneValueArgs STD DEFAULTS_VERSION )
    set( multiValueArgs SOURCES EXTRA_OPTIONS )
    cmake_parse_arguments( "SCOPE" "${optionArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
    if( SCOPE_UNPARSED_ARGUMENTS )
        list( JOIN SCOPE_UNPARSED_ARGUMENTS "\", \"" SCOPE_UNPARSED_ARGUMENTS_STR )
        list( JOIN "${oneValueArgs};${multiValueArgs}" "\", \"" POSSIBLE_ARGUMENTS_STR )
        message( SEND_ERROR "make_test_target(): Invalid argument keyword(s) \"${SCOPE_UNPARSED_ARGUMENTS_STR}\"; expected one of \"${POSSIBLE_ARGUMENTS_STR}\"" )
    endif()
    if( SCOPE_KEYWORDS_MISSING_VALUES )
        list( JOIN SCOPE_KEYWORDS_MISSING_VALUES "\", \"" SCOPE_KEYWORDS_MISSING_VALUES_STR )
        message( SEND_ERROR "make_test_target(): argument keyword(s) \"${SCOPE_KEYWORDS_MISSING_VALUES_STR}\" missing values" )
    endif()
    if( NOT SCOPE_SOURCES )
        message( SEND_ERROR "make_test_target(): no argument specified for SCOPE_SOURCES" )
    endif()
    if( NOT SCOPE_DEFAULTS_VERSION )
        message( SEND_ERROR "make_test_target(): no argument specified for DEFAULTS_VERSION" )
    endif()

    message( STATUS "Make target: '${target}'" )

    add_executable( ${target} ${SCOPE_SOURCES} )

    set( localOptions ${OPTIONS} )
    set( localDefinitions ${GSL_CONFIG} )

    if( MSVC )
        list( APPEND localOptions "/WX" "/W4" )
        list( APPEND localDefinitions "_SCL_SECURE_NO_WARNINGS" )
        if( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 17.0 ) # VC++ 2010 and earlier
            list( APPEND localOptions "/wd4275" ) # suppress C4275: non dll-interface class 'stdext::exception' used as base for dll-interface class 'std::bad_cast'
        endif()
    elseif( CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang" )
        list( APPEND localOptions
            "-Werror"
            "-Wall"
            "-Wextra"
            #"-Wno-missing-braces"
            "-Wconversion"
            "-Wsign-conversion"
            "-fno-elide-constructors"
            "-fstrict-aliasing" "-Wstrict-aliasing=2"
        )
        if( NOT SCOPE_CUDA )
            list( APPEND localOptions "-pedantic" ) # NVCC and "-pedantic" don't mix (GCC complains that NVCC-generated GCC-specific code is GCC specific)
        endif()
        if( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
            list( APPEND localOptions "-Wno-long-long" ) # irrelevant strict-C++98 warning about non-standard type `long long`
            if( CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8 )
                list( APPEND localOptions "-Wno-type-limits" ) # irrelevant warning about `unsigned value < 0` comparison
            endif()
        elseif( CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang" )
            list( APPEND localOptions "-Wno-c++11-long-long" ) # irrelevant strict-C++98 warning about non-standard type `long long`
        endif()
    endif()

    if( SCOPE_NO_EXCEPTIONS )
        if( MSVC )
            list( APPEND localOptions "/EHs-" )
            list( APPEND localDefinitions "_HAS_EXCEPTIONS=0" )
        elseif( CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang" )
            list( APPEND localOptions "-fno-exceptions" )
        endif()
    else()
        list( APPEND localDefinitions "gsl_CONFIG_CONTRACT_VIOLATION_THROWS" )
        if( MSVC )
            list( APPEND localOptions "/EHsc" )
        endif()
    endif()

    if( SCOPE_STD )
        if( MSVC )
            list( APPEND localOptions "/std:c++${SCOPE_STD}" )
        else()
            list( APPEND localOptions "-std=c++${SCOPE_STD}" )
        endif()
    endif()

    if( SCOPE_CUDA )
        list( TRANSFORM localOptions PREPEND "${HOST_COMPILER_PREFIX}" )
        target_compile_options( ${target} PRIVATE ${CUDA_OPTIONS} )
        if( SCOPE_STD )
            set_target_properties( ${target} PROPERTIES CUDA_STANDARD ${SCOPE_STD} )
        endif()
    endif()

    target_compile_options( ${target} PRIVATE ${localOptions} ${SCOPE_EXTRA_OPTIONS} )
    target_compile_definitions( ${target} PRIVATE ${localDefinitions} )
    target_link_libraries( ${target} PRIVATE ${PACKAGE}-${SCOPE_DEFAULTS_VERSION} )

    if( NOT SCOPE_NO_PCH
            AND NOT CMAKE_VERSION VERSION_LESS 3.16  # VERSION_GREATER_EQUAL doesn't exist in CMake 3.5
            AND NOT ( CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_SYSTEM_NAME MATCHES "Darwin" ) )  # and GCC on MacOS has trouble with addresses of some text segments in the PCH
        target_precompile_headers( ${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/gsl-lite.t.hpp )
    endif()

    if( NOT SCOPE_NO_EXCEPTIONS AND NOT SCOPE_COMPILE_ONLY )
        # We only add tests for targets with exceptions enabled. lest has been modified to permit compilation without exceptions
        # so we can test compiling gsl-lite without exceptions, but the no-exception tests will not run correctly because lest
        # relies on exceptions for running tests and therefore cannot function correctly without.
        add_test( NAME ${target} COMMAND ${target} )
    endif()

endfunction()
