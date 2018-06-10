cmake_minimum_required( VERSION 3.0 )

project( program-using-gsl-lite LANGUAGES CXX )

# Make program executable:

set( SOURCES main.cpp)
add_executable( program ${SOURCES} )
target_link_libraries( program PRIVATE gsl )
