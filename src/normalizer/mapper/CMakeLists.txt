include_directories("${CMAKE_SOURCE_DIR}/src")
add_library(Mapper SHARED mapper.cpp)
install(TARGETS Mapper
        DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/normalizer")
set(NORMALIZER_LIBRARIES ${NORMALIZER_LIBRARIES} Mapper PARENT_SCOPE)
install_headers(mapper.h)

