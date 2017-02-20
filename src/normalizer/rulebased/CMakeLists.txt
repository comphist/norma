include_directories("${CMAKE_SOURCE_DIR}/src")
add_library(RuleBased SHARED
            rule.cpp rule_learn.cpp rule_collection.cpp
            candidate_finder.cpp rulebased.cpp)
target_link_libraries(RuleBased LINK_PUBLIC norma)
install(TARGETS RuleBased
        DESTINATION "${NORMA_DEFAULT_PLUGIN_BASE}")
install_headers(candidate_finder.h rule.h rule_collection.h rule_learn.h
                rulebased.h symbols.h)
set(NORMALIZER_LIBRARIES ${NORMALIZER_LIBRARIES} RuleBased PARENT_SCOPE)

