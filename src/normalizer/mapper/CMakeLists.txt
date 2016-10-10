include_directories("${CMAKE_SOURCE_DIR}/src")
add_library(Mapper SHARED mapper.cpp)
install(TARGETS Mapper
        DESTINATION "${NORMA_DEFAULT_PLUGIN_BASE}")
set(NORMALIZER_LIBRARIES ${NORMALIZER_LIBRARIES} Mapper PARENT_SCOPE)
install_headers(mapper.h)

