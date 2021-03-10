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
    -Dgsl_TESTING_
    -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS
    -Dgsl_CONFIG_CONTRACT_CHECKING_AUDIT
)

set( CUDA_CONFIG )

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

set( OPTION_PREFIX "" )

# Determine compiler-specifics for MSVC, GNUC, Clang:

if( MSVC )
    set( HAS_STD_FLAGS TRUE )

    # remove "/Wx" from CMAKE_CXX_FLAGS if present, as VC++ doesn't tolerate more than one "/Wx" flag
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" " " CMAKE_CXX_FLAGS_NEW "${CMAKE_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_NEW}")
    endif()

    set( OPTIONS     /EHsc /WX /W4 )
    set( DEFINITIONS -D_SCL_SECURE_NO_WARNINGS )

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
    # as is
    message( STATUS "Matched: Intel ${CMAKE_CXX_COMPILER_VERSION}" )
else()
    # as is
    message( STATUS "Matched: nothing" )
endif()

# Determine compiler-specifics for NVCC:

get_property( _languages GLOBAL PROPERTY ENABLED_LANGUAGES )
if( CUDA IN_LIST _languages )
    if( CMAKE_CUDA_COMPILER_ID MATCHES "NVIDIA" )
        message( STATUS "Matched: NVCC ${CMAKE_CUDA_COMPILER_VERSION}" )

        # Set NVCC-specific options:
        set( OPTION_PREFIX "-Xcompiler=" )
        set( CUDA_CONFIG ${CUDA_CONFIG} --Werror all-warnings -G )

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

function( make_test_target target sources std extraOptions defaultsVersion )
    message( STATUS "Make target: '${target}'" )

    add_executable            ( ${target} ${sources}  )
    target_link_libraries     ( ${target} PRIVATE ${PACKAGE}-${defaultsVersion} )
    target_compile_options    ( ${target} PRIVATE ${OPTIONS} ${extraOptions} )
    target_compile_definitions( ${target} PRIVATE ${DEFINITIONS} ${GSL_CONFIG} )

    if( NOT CMAKE_VERSION VERSION_LESS 3.16  # VERSION_GREATER_EQUAL doesn't exist in CMake 3.5
            AND NOT ( CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_SYSTEM_NAME MATCHES "Darwin" ) )  # and GCC on MacOS has trouble with addresses of some text segments in the PCH
        target_precompile_headers( ${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/gsl-lite.t.hpp )
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


function( make_cuda_test_target target sources std extraOptions )
    message( STATUS "Make CUDA target: '${target}'" )

    set( prefixedOptions ${OPTIONS} )
    list( TRANSFORM prefixedOptions PREPEND "${OPTION_PREFIX}" )

    add_executable            ( ${target} ${sources} )
    target_link_libraries     ( ${target} PRIVATE ${PACKAGE}-v1 )
    target_compile_options    ( ${target} PRIVATE ${CUDA_OPTIONS} ${prefixedOptions} ${extraOptions} )
    target_compile_definitions( ${target} PRIVATE ${DEFINITIONS} ${GSL_CONFIG} )
    #target_compile_features   ( ${target} PRIVATE cxx_std_${langVersion} cuda_std_${langVersion} )  # apparently not supported yet 
    set_target_properties     ( ${target} PROPERTIES CUDA_STANDARD ${std} )
    target_precompile_headers ( ${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/gsl-lite.t.hpp )

    add_test( NAME ${target} COMMAND ${target} )

endfunction()
