if( NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION )
    cmake_minimum_required( VERSION 3.5 FATAL_ERROR )
endif()

project( program-using-gsl-lite LANGUAGES CXX )

# Make program executable:

set( SOURCES main.cpp)
add_executable( program ${SOURCES} )
target_link_libraries( program PRIVATE gsl )
