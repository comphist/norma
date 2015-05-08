include_directories("${CMAKE_SOURCE_DIR}/src")
add_library(mapper SHARED mapper.cpp)
install(TARGETS mapper
        DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/normalizer")
set(NORMALIZER_LIBRARIES ${NORMALIZER_LIBRARIES} mapper PARENT_SCOPE)
install_headers(mapper.h)

