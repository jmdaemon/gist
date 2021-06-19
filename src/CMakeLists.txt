set(HEADER_LIST "${GIST_SOURCE_DIR}/include/util")
set(TARGET gist)

add_executable(${TARGET})
target_sources(${TARGET}
    PRIVATE Gist.cpp
    PUBLIC  ${HEADER_LIST}
)

target_include_directories(${TARGET} PUBLIC ${HEADER_LIST})
#target_link_libraries(${TARGET} PRIVATE fmt cpr)
target_link_libraries(${TARGET} PRIVATE fmt httplib)
target_compile_features(${TARGET} PUBLIC cxx_std_20)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
