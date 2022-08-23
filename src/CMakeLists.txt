set(HEADERS ${INCLUDES_DIR} ${INCLUDES_DIR}/utility "${PROJECT_BINARY_DIR}/include" ${jolly_SOURCE_DIR}/include)
set(TARGET gist)

set(LIB_NAME gist)
add_library(${LIB_NAME} gist.cpp)
target_include_directories(${LIB_NAME} PUBLIC ${HEADERS})
target_link_libraries(${LIB_NAME} PRIVATE pcre utility jolly spdlog tomlplusplus nlohmann_json restclient-cpp)

set(TARGET gist-oldbin)
add_executable(${TARGET} Gist.cpp Data.cpp Api.cpp)
target_include_directories(${TARGET} PUBLIC ${HEADERS})
target_link_libraries(${TARGET} PRIVATE
    tomlplusplus nlohmann_json restclient-cpp CLI11::CLI11 fmt spdlog pcre utility jolly)
