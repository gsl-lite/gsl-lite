cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

# gsl-lite version, updated by script/update-version.py:

set( gsl_lite_version "0.32" )

find_package( gsl-lite ${gsl_lite_version} REQUIRED )

project( "package_using_gsl-lite" LANGUAGES CXX )

add_executable(        "package_using_gsl-lite" main.cpp )
target_link_libraries( "package_using_gsl-lite" PRIVATE gsl-lite::gsl-lite )

enable_testing()
add_test( NAME test COMMAND package_using_gsl-lite )
