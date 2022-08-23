set(HEADERS ${INCLUDES_DIR} ${INCLUDES_DIR}/utility "${PROJECT_BINARY_DIR}/include")
set(TARGET gist)

add_executable(${TARGET} Gist.cpp Data.cpp Str.cpp Api.cpp)
target_include_directories(${TARGET} PUBLIC ${HEADERS})
target_link_libraries(${TARGET} PRIVATE
    fmt tomlplusplus nlohmann_json restclient-cpp CLI11::CLI11 argparse spdlog pcre utility)
