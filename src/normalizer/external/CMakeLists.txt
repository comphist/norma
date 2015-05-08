include_directories("${CMAKE_SOURCE_DIR}/src")
add_library(external SHARED
            external.cpp)
set(NORMALIZER_LIBRARIES ${NORMALIZER_LIBRARIES} external PARENT_SCOPE)
install_headers(external.h)

