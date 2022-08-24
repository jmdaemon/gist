# Unit Tests
set(HEADERS ${INCLUDES_DIR} ${DOCTEST_INCLUDE_DIR} ${jolly_SOURCE_DIR}/include ${utility_SOURCE_DIR}/include)
set(TARGET gist)

# Add unit tests
set(TEST_NAME test-${TARGET})
add_executable(${TEST_NAME} test_gist.cpp)
target_include_directories(${TEST_NAME} PUBLIC ${HEADERS})
target_link_libraries(${TEST_NAME} PRIVATE ${PKG_DEPENDS} doctest gist cli)
add_test(
    NAME ${TEST_NAME}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMAND ${TEST_NAME})

if (DISCOVER_TESTS)
    # Include doctest cmake module
    include(${DOCTEST_SOURCE_DIR}/scripts/cmake/doctest.cmake)

    # Discover unit tests
    doctest_discover_tests(${TEST_NAME})
endif()
