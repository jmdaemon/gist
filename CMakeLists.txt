cmake_minimum_required(VERSION 3.19) # CMake 3.9

project (gist
    VERSION 0.2.0
    DESCRIPTION "Gist CLI tool to interact with the Gist REST api"
    LANGUAGES CXX
    )

# Variable Toggles
if (NOT USE_AS_SUBPROJECT)
    set(NO_COLOR OFF CACHE INTERNAL "Toggles colored output errors")
    set(BUILD_DOC OFF CACHE INTERNAL "Build documentation")
    set(ENABLE_LOGGING OFF CACHE INTERNAL "View the logging statements/debug info")
    set(LOG_LEVEL 0 CACHE INTERNAL "Enable only specific log levels")
endif()

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if (EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with "
                        "CMakeLists.txt file). Please make a build subdirectory. Feel free to "
                        "remove CMakeCache.txt and CMakeFiles.")
endif()

set( GIST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( GIST_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )

# Include CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/modules/" "${PROJECT_SOURCE_DIR}/cmake")

# Single source project version
configure_file (
  "${PROJECT_SOURCE_DIR}/include/Version.h.in"
  "${PROJECT_BINARY_DIR}/include/Version.h")

# Include dependencies
include(FetchContent)

include(cli11)
include(spdlog)
include(restclient-cpp)
include(tomlplusplus)
include(json)
include(argparse)
include(utility)

# Add subdirectories
add_subdirectory(src)

if (BUILD_TESTING AND NOT USE_AS_SUBPROJECT)
    include(doctest)
    include(MakeTests)
    include(CTest)
    add_subdirectory(tests)
endif()

# Package for debian
include(Debian)

set(CMAKE_INSTALL_PREFIX /usr/local)
install(TARGETS ${OGGEX} DESTINATION bin)
install(TARGETS ${OGGEX} CONFIGURATIONS Debug   RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin/Debug)
install(TARGETS ${OGGEX} CONFIGURATIONS Release RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin/Release)
