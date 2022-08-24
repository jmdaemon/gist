set(HEADERS ${INCLUDES_DIR} ${jolly_SOURCE_DIR}/include)
set(TARGET gist)

set(LIB_NAME gist)
add_library(${LIB_NAME} gist.cpp)
target_include_directories(${LIB_NAME} PUBLIC ${HEADERS})
target_link_libraries(${LIB_NAME} PRIVATE ${PKG_DEPENDS})
