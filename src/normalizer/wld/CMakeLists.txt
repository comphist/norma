include_directories("${CMAKE_SOURCE_DIR}/src")
add_library(WLD SHARED
            symbols.cpp weight_set.cpp
            levenshtein_algorithm.cpp levenshtein_aligner.cpp
            wld.cpp)
install(TARGETS WLD
        DESTINATION "${NORMA_DEFAULT_PLUGIN_BASE}")
set(NORMALIZER_LIBRARIES ${NORMALIZER_LIBRARIES} WLD PARENT_SCOPE)
install_headers(levenshtein_algorithm.h levenshtein_aligner.h symbols.h
                typedefs.h weight_set.h wld.h)

