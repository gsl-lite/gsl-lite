# Copyright 2015-2018 by Martin Moene
#
# gsl-lite is based on GSL: Guidelines Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT).

cmake_minimum_required( VERSION 3.17 FATAL_ERROR )

project( gsl-lite-test-cuda LANGUAGES CXX CUDA )

set( unit_name "gsl" )
set( PACKAGE   ${unit_name}-lite )
set( PROGRAM   ${unit_name}-lite )

message( STATUS "Subproject '${PROJECT_NAME}', programs '${PROGRAM}-*'")

include(${CMAKE_CURRENT_SOURCE_DIR}/../MakeTestTarget.cmake)


function(make_cuda_test_targets nameRoot srcs maxStd extraOptions )
    if( HAS_CUDA14_FLAG AND NOT maxStd LESS 14 )
        make_cuda_test_target( ${nameRoot}-cuda14.t "${srcs}" 14 "${extraOptions}" )
    endif()

    if( HAS_CUDA17_FLAG AND NOT maxStd LESS 17 )
        make_cuda_test_target( ${nameRoot}-cuda17.t "${srcs}" 17 "${extraOptions}" )
    endif()
endfunction()


# Add test targets for all CUDA versions :

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

make_cuda_test_targets( "${PROGRAM}" "${SOURCES};${CUDA_SOURCES}" 99 "" )
make_cuda_test_targets( "${PROGRAM}-expt" "${SOURCES};${CUDA_SOURCES}" 99 "--expt-relaxed-constexpr" )


# end of file
