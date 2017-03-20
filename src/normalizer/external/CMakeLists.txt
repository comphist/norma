include_directories("${CMAKE_SOURCE_DIR}/src")
add_library(External SHARED
            external.cpp)
install(TARGETS External
        DESTINATION "${NORMA_DEFAULT_PLUGIN_BASE}")
set(NORMALIZER_LIBRARIES ${NORMALIZER_LIBRARIES} External PARENT_SCOPE)
install_headers(external.h)

