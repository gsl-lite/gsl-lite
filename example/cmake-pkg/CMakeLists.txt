if( NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION )
    cmake_minimum_required( VERSION 3.5 FATAL_ERROR )
endif()

# gsl-lite version, updated by script/update-version.py:

set( gsl_lite_version "0.35" )

find_package( gsl-lite ${gsl_lite_version} REQUIRED )

project( "package_using_gsl-lite" LANGUAGES CXX )

add_executable(        "package_using_gsl-lite" main.cpp )
target_link_libraries( "package_using_gsl-lite" PRIVATE gsl::gsl-lite )

enable_testing()
add_test( NAME test COMMAND package_using_gsl-lite )
