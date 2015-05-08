include_directories("${CMAKE_SOURCE_DIR}/src")
add_library(mapper SHARED mapper.cpp)
set(NORMALIZER_LIBRARIES ${NORMALIZER_LIBRARIES} mapper PARENT_SCOPE)
install_headers(mapper.h)

