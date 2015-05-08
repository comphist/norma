include_directories("${CMAKE_SOURCE_DIR}/src")
add_library(rulebased SHARED
            rule.cpp rule_learn.cpp rule_collection.cpp
            candidate_finder.cpp rulebased.cpp)
install(TARGETS rulebased
        DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/normalizer")
install_headers(candidate_finder.h rule.h rule_collection.h rule_learn.h
                rulebased.h symbols.h)
set(NORMALIZER_LIBRARIES ${NORMALIZER_LIBRARIES} rulebased PARENT_SCOPE)

