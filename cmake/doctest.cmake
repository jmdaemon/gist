
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

