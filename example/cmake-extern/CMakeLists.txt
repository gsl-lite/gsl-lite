if( NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION )
    cmake_minimum_required( VERSION 3.5 FATAL_ERROR )
endif()

project( use-gsl-lite LANGUAGES CXX )

# Set default ExternalProject root directory and add gsl-lite:

set( GSL_LITE_URL https://github.com/martinmoene/gsl-lite.git )

include( ExternalProject )
find_package( Git REQUIRED )

set_directory_properties( PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/3rd_party )

ExternalProject_Add(
    gsl-extern
    GIT_REPOSITORY ${GSL_LITE_URL}
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
   )

# Provide #include access to gsl-lite as 'gsl/gsl' and as 'gsl/gsl-lite.hpp': 

ExternalProject_Get_Property( gsl-extern SOURCE_DIR )
set( GSL_LITE_INCLUDE_DIR ${SOURCE_DIR}/include CACHE INTERNAL "Include folder for gsl-lite")

add_library( gsl INTERFACE )
target_include_directories( gsl INTERFACE ${GSL_LITE_INCLUDE_DIR} )

# Build program from src:

add_subdirectory( src ) 
