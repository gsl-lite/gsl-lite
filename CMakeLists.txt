# Copyright 2015 by Martin Moene
#
# gsl-lite is based on GSL: Guideline Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT). 
project( gsl_lite )
cmake_minimum_required( VERSION 3.2 )

enable_testing()

add_subdirectory( test )
#add_subdirectory( example )

add_library(gsl_lite INTERFACE)
target_include_directories(gsl_lite 
    INTERFACE 
        $<BUILD_INTERFACE:include>
)

install(
    TARGETS
        gsl_lite
    EXPORT
        gsl_lite-config
)

install(
    DIRECTORY
        include/
    DESTINATION
        include
)

install(
    EXPORT
        gsl_lite-config
    DESTINATION
        lib/gsl_lite
)

# end of file
