# Copyright 2015-2018 by Martin Moene
#
# gsl-lite is based on GSL: Guidelines Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT).

cmake_minimum_required( VERSION 3.15 FATAL_ERROR )

project( gsl-lite-test-cuda LANGUAGES CXX CUDA )

set( unit_name "gsl" )
set( PACKAGE   ${unit_name}-lite )
set( PROGRAM   ${unit_name}-lite )

message( STATUS "Subproject '${PROJECT_NAME}', programs '${PROGRAM}-*'")

# Test sources:

set( CUDA_SOURCES
    cuda.t.cu
)
set( SOURCES
    ../gsl-lite.t.cpp
    ../assert.t.cpp
    ../at.t.cpp
    ../byte.t.cpp
    ../emulation.t.cpp
    ../issue.t.cpp
    ../not_null.t.cpp
    ../owner.t.cpp
    ../span.t.cpp
    ../string_span.t.cpp
    ../util.t.cpp
)
set_property( SOURCE ${SOURCES} PROPERTY LANGUAGE CUDA ) # compile .cpp files with the CUDA compiler
# Configure gsl-lite for testing:

set( GSL_CONFIG
    -Dgsl_TESTING_
    -Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS
    -Dgsl_CONFIG_CONTRACT_CHECKING_AUDIT
)
# Set NVCC-specific options:
set( NVCC_OPTIONS --Werror all-warnings -G )

# Determine compiler-specifics for MSVC, GNUC, Clang:

if( MSVC )
    message( STATUS "Matched: MSVC")

    set( OPTIONS     /EHsc /WX /W4 )
    set( DEFINITIONS -D_SCL_SECURE_NO_WARNINGS )

elseif( CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang" )
    message( STATUS "CompilerId: '${CMAKE_CXX_COMPILER_ID}'")

    set( OPTIONS ${OPTIONS}
        -Werror
        -Wall
        -Wno-missing-braces
        -Wconversion
        -Wsign-conversion
        -fno-elide-constructors
        -fstrict-aliasing -Wstrict-aliasing=2
    )
    set( DEFINITIONS "" )

else()
    # as is
    message( STATUS "Matched: nothing")
endif()

# Have NVCC forward the host compiler options with "-Xcompiler=<opt>:

list( TRANSFORM OPTIONS PREPEND -Xcompiler= )

# Add targets and configure unit tests via CTest:

enable_testing()

function( make_cuda_test_target target sources extraOptions )
    message( STATUS "Make CUDA target: '${extraOptions}'" )

    add_executable            ( ${target} ${sources} )
    target_link_libraries     ( ${target} PRIVATE ${PACKAGE}-v1 )
    target_compile_options    ( ${target} PRIVATE ${NVCC_OPTIONS} ${OPTIONS} ${extraOptions} )
    target_compile_definitions( ${target} PRIVATE ${DEFINITIONS} ${GSL_CONFIG} )
    target_compile_features   ( ${target} PRIVATE cxx_std_14 )
    set_target_properties     ( ${target} PROPERTIES CUDA_STANDARD 14 )
    if( CMAKE_VERSION VERSION_GREATER_EQUAL 3.16 )
        target_precompile_headers( ${target} PRIVATE ../gsl-lite.t.hpp )
    endif()

    add_test( NAME ${target} COMMAND ${target} )

endfunction()

make_cuda_test_target( ${PROGRAM}-cuda.t "${SOURCES};${CUDA_SOURCES}" "" )
make_cuda_test_target( ${PROGRAM}-cuda-expt.t "${SOURCES};${CUDA_SOURCES}" "--expt-relaxed-constexpr" )

# end of file
