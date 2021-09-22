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

FetchContent_Declare(restclient-cpp
    GIT_REPOSITORY https://github.com/mrtazz/restclient-cpp)
FetchContent_MakeAvailable(restclient-cpp)

FetchContent_Declare(tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v2.4.0)
FetchContent_MakeAvailable(tomlplusplus)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git
  GIT_TAG v3.9.1)
FetchContent_GetProperties(json)

if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse)
FetchContent_MakeAvailable(argparse)

if (BUILD_TESTING)
include(ExternalProject)
  find_package(Git REQUIRED)
  ExternalProject_Add( # https://github.com/onqtam/doctest/blob/master/doc/markdown/build-systems.md
      doctest
      PREFIX ${CMAKE_BINARY_DIR}/doctest
      GIT_REPOSITORY https://github.com/onqtam/doctest.git
      TIMEOUT 10
      UPDATE_COMMAND ${GIT_EXECUTABLE} pull
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
      LOG_DOWNLOAD ON
  )
  
  ExternalProject_Get_Property(doctest source_dir) # Expose required variable (DOCTEST_INCLUDE_DIR) to parent scope
  set(DOCTEST_INCLUDE_DIR ${source_dir}/doctest CACHE INTERNAL "Path to include folder for doctest")
endif()

set(CMAKE_CXX_FLAGS -fdiagnostics-color=always)
set(CMAKE_C_FLAGS -fdiagnostics-color=always)

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# Build .deb file
include(CPack)
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Joseph Diza") # required

set(CMAKE_INSTALL_PREFIX /usr/local)
install(TARGETS ${OGGEX} DESTINATION bin)
install(TARGETS ${OGGEX} CONFIGURATIONS Debug   RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/Debug/bin)
install(TARGETS ${OGGEX} CONFIGURATIONS Release RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/Release/bin)
