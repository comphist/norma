add_sources(lexicon.cpp)
install_headers(lexicon.h lexicon_interface.h)
include_directories("${CMAKE_SOURCE_DIR}/src")
add_executable(norma_lexicon lexicon_main.cpp)
target_link_libraries(norma_lexicon norma ${Boost_PROGRAM_OPTIONS_LIBRARY})
install(TARGETS norma_lexicon
    DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
