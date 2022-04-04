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

# Project Dependencies  : fmt, httplib, tomlplusplus
# Optional Dependencies : doctest (unit testing), Cap'n'Proto, PkgConfig, standalone asio (installed via package manager)
include(FetchContent)
FetchContent_Declare(libfmt
    GIT_REPOSITORY "https://github.com/fmtlib/fmt")
FetchContent_MakeAvailable(libfmt)

# Configure gist project versions
configure_file (
  "${PROJECT_SOURCE_DIR}/include/Version.h.in"
  "${PROJECT_BINARY_DIR}/include/Version.h"
)

#include_directories("${PROJECT_BINARY_DIR}/include")

#find_package(PkgConfig REQUIRED)
#find_package(CapnProto CONFIG REQUIRED)

find_package(CLI11 CONFIG REQUIRED)

include(restclient-cpp)
include(tomlplusplus)
include(json)
include(argparse)
include(doctest)

set(CMAKE_CXX_FLAGS -fdiagnostics-color=always)
set(CMAKE_C_FLAGS -fdiagnostics-color=always)

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Joseph Diza") # required
set(CPACK_PACKAGE_DIRECTORY "packages/${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}")
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "restclient-cpp")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_GENERATOR "DEB;TGZ;ZIP") # Create .deb, .tgz, .zip files
include(CPack)

set(CMAKE_INSTALL_PREFIX /usr/local)
install(TARGETS ${OGGEX} DESTINATION bin)
install(TARGETS ${OGGEX} CONFIGURATIONS Debug   RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin/Debug)
install(TARGETS ${OGGEX} CONFIGURATIONS Release RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin/Release)
