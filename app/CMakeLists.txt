set(HEADERS ${INCLUDES_DIR} ${HEADERS_VERSION} ${jolly_SOURCE_DIR}/include)
set(TARGET gist)

# Make static, shared library
make_ssl(NAME cli
    SRCS cli.c
    HDRS ${HEADERS})

set(BIN_NAME ${TARGET}-bin)
add_executable(${BIN_NAME} gist-bin.cpp)
target_include_directories(${BIN_NAME} PUBLIC ${HEADERS})
target_link_libraries(${BIN_NAME} PRIVATE cli)

# Rename binary executable without -bin suffix
set_target_properties(${BIN_NAME} PROPERTIES OUTPUT_NAME ${TARGET})

# Manual Installation
install(TARGETS ${BIN_NAME}
    DESTINATION bin
    COMPONENT bin)
