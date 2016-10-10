add_sources(lexicon.cpp)
install_headers(lexicon.h lexicon_interface.h)
include_directories("${CMAKE_SOURCE_DIR}/src")
add_executable(norma_lexicon lexicon_main.cpp)
target_link_libraries(norma_lexicon norma ${Boost_PROGRAM_OPTIONS_LIBRARY})

# should probably use $NORMA_BINDIR as install target, but this would require
# reordering the main CMakeLists.txt (this file is included before NORMA_BINDIR
# is defined) and probably making it a CACHE variable
if (NOT CMAKE_INSTALL_BINDIR)
    set(CMAKE_INSTALL_BINDIR "bin")
endif()
install(TARGETS norma_lexicon DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
