cmake_minimum_required(VERSION 3.19) # CMake 3.9

project (gist
    VERSION 0.2.0
    DESCRIPTION "Gist CLI tool to interact with the Gist REST api"
    LANGUAGES CXX
    )

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) 
    set(CMAKE_CXX_STANDARD 20) 
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_CXX_STANDARD_REQUIRED True) 
    set(CMAKE_CXX_EXTENSIONS OFF) 
    set_property(GLOBAL PROPERTY USE_FOLDERS ON) 
endif()

if (NOT COLOR)
    # Don't use colored output
else()
    # Include colored output by default
    include(Color)
endif()

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with "
                        "CMakeLists.txt file). Please make a build subdirectory. Feel free to "
                        "remove CMakeCache.txt and CMakeFiles.")
endif()

set( GIST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( GIST_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/modules/" "${PROJECT_SOURCE_DIR}/cmake" )

include(FetchContent)

# Configure project versions
configure_file (
  "${PROJECT_SOURCE_DIR}/include/Version.h.in"
  "${PROJECT_BINARY_DIR}/include/Version.h"
)

# Include dependencies
include(cli11)
include(spdlog)
include(restclient-cpp)
include(tomlplusplus)
include(json)
include(argparse)
include(doctest)

# Add subdirectories
add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# Package for debian
include(Debian)

set(CMAKE_INSTALL_PREFIX /usr/local)
install(TARGETS ${OGGEX} DESTINATION bin)
install(TARGETS ${OGGEX} CONFIGURATIONS Debug   RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin/Debug)
install(TARGETS ${OGGEX} CONFIGURATIONS Release RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin/Release)
