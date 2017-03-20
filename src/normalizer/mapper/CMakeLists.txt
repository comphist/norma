include_directories("${CMAKE_SOURCE_DIR}/src")
add_library(Mapper SHARED mapper.cpp)
target_link_libraries(Mapper LINK_PUBLIC norma)
install(TARGETS Mapper
        DESTINATION "${NORMA_DEFAULT_PLUGIN_BASE}")
set(NORMALIZER_LIBRARIES ${NORMALIZER_LIBRARIES} Mapper PARENT_SCOPE)
install_headers(mapper.h)

