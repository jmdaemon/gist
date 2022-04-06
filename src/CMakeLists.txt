set(INCLUDES_DIR "${PROJECT_SOURCE_DIR}/include")
set(HEADER_LIST ${INCLUDES_DIR} ${INCLUDES_DIR}/utility "${PROJECT_BINARY_DIR}/include")
set(TARGET gist)

add_executable(${TARGET})
target_sources(${TARGET}
    PRIVATE Gist.cpp Data.cpp Str.cpp
    PUBLIC  ${HEADER_LIST}
)

#target_include_directories(${TARGET} PUBLIC ${HEADER_LIST} ${UTILITY_HEADERS})
target_include_directories(${TARGET} PUBLIC ${HEADER_LIST})
target_link_libraries(${TARGET} PRIVATE
    fmt tomlplusplus nlohmann_json restclient-cpp CLI11::CLI11 argparse spdlog utility)
target_compile_features(${TARGET} PUBLIC cxx_std_20)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
