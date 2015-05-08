include_directories("${CMAKE_SOURCE_DIR}/src")
add_library(wld SHARED
            symbols.cpp weight_set.cpp
            levenshtein_algorithm.cpp levenshtein_aligner.cpp
            wld.cpp)
install(TARGETS wld
        DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/normalizer")
set(NORMALIZER_LIBRARIES ${NORMALIZER_LIBRARIES} wld PARENT_SCOPE)
install_headers(levenshtein_algorithm.h levenshtein_aligner.h symbols.h
                typedefs.h weight_set.h wld.h)

