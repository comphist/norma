include_directories("${CMAKE_SOURCE_DIR}/src")
add_library(external SHARED
            external.cpp)
install(TARGETS external
        DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/normalizer")
set(NORMALIZER_LIBRARIES ${NORMALIZER_LIBRARIES} external PARENT_SCOPE)
install_headers(external.h)

