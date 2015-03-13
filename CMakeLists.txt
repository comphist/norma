cmake_minimum_required(VERSION 2.8.10)
project(norma)
# query git tags to set version
execute_process(
    COMMAND sh GIT-VERSION-GEN
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE CMAKE_PROJECT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
# read command line arguments to cmake
set(STRING_IMPL "ICU" CACHE STRING "String implementation to use (ICU|STD)")
set(DESTINATION "/usr/local/" CACHE STRING "Install destination")
set(USE_PYTHON OFF CACHE BOOL "Whether to make Python buildings/embed")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")
include(AddSources)
include(cpplint)
include(CTAGS)
enable_testing()

find_package(Boost 1.50
    COMPONENTS program_options unit_test_framework regex system filesystem
    REQUIRED)
find_package(Gfsm REQUIRED)
include_directories(${LIBGFSM_INCLUDE_DIRS})

############################ string implementation #############################
if (STRING_IMPL STREQUAL "ICU")
    find_package(ICU 49)
    if (NOT ICU_FOUND)
        set(STRING_IMPL "STD")
        message(SEND_ERROR "ICU Implementation NOT found - defaulting to use std::string")
    else()
        message(STATUS "Using ICU Unicode string implementation")
        set(STRING_LIBRARY ${ICU_LIBRARIES})
        set (USE_ICU_STRING TRUE)
        include_directories(${ICU_INCLUDE_DIRS})
    endif()
elseif(STRING_IMPL STREQUAL "STD")
    message(STATUS "Using std::string implementation")
else()
    message(FATAL_ERROR "string implementation ${STRING_IMPL} not recognized!")
endif()

########################## python bindings #####################################
if(USE_PYTHON)
    find_package(PythonLibs 2.7 REQUIRED)
    find_package(PythonInterp 2.7 REQUIRED)
    include_directories(${PYTHON_INCLUDE_DIRS})
    set (EMBED_PYTHON TRUE)
endif()

######################### configured headers ###################################
set( NORMA_TEST_BASE_DIR "${CMAKE_SOURCE_DIR}/src/tests/data" )
configure_file( "${CMAKE_SOURCE_DIR}/src/config.h.in"
                "${CMAKE_BINARY_DIR}/src/config.h" )
configure_file( "${CMAKE_SOURCE_DIR}/src/defines.h.in"
                "${CMAKE_BINARY_DIR}/src/defines.h")
include_directories( "${CMAKE_BINARY_DIR}/src" )

############################ compiler flags  ###################################
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -pedantic")
    if(${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER "4.7.99")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else()
        message(FATAL_ERROR "You need at least g++ >= 4.8! Your version is ${CMAKE_CXX_COMPILER_VERSION}")
    endif()
    set(CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fno-inline")
endif()
if (MSVC)
    message(FATAL_ERROR "Sorry, compilation won't work with MSVC due to incomplete C++11 support")
endif()

####################### main source directories ################################
add_subdirectory(src)
add_subdirectory(src/interface)
add_subdirectory(src/normalizer)
add_subdirectory(src/normalizer/wld)
add_subdirectory(src/normalizer/rulebased)
add_subdirectory(src/gfsm)
include_directories(src src/normalizer src/interface src/gfsm)

if(USE_PYTHON)
    add_subdirectory(src/normalizer/external)
    add_subdirectory(src/python)
endif()

############################## library #########################################
add_library(norma SHARED ${SRCS})
target_link_libraries(norma
    LINK_PUBLIC
    ${STRING_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    LINK_PRIVATE
    ${PYTHON_LIBRARIES}
    ${Boost_REGEX_LIBRARY}
    ${LIBGFSM_LIBRARIES}
)

############################ executables #######################################
add_executable(normalize src/main.cpp)
target_link_libraries(normalize norma ${Boost_PROGRAM_OPTIONS_LIBRARY})
add_executable(norma_lexicon src/normalizer/lexicon_main.cpp)
target_link_libraries(norma_lexicon norma ${Boost_PROGRAM_OPTIONS_LIBRARY})

############################### tests ##########################################
add_subdirectory(src/tests)

if (CMAKE_CONFIGURATION_TYPES)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
        --force-new-ctest-process --output-on-failure
        --build-config "$<CONFIGURATION>")
else()
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
        --force-new-ctest-process --output-on-failure)
endif()

############################ installation ######################################
# library
install(TARGETS norma DESTINATION "${DESTINATION}/lib")
# binary
install(TARGETS normalize DESTINATION "${DESTINATION}/bin")
install(TARGETS norma_lexicon DESTINATION "${DESTINATION}/bin")
# docs/examples
set(docdest "${DESTINATION}/share/doc/norma-${CMAKE_PROJECT_VERSION}")
install(FILES "${CMAKE_SOURCE_DIR}/README.md"
              "${CMAKE_SOURCE_DIR}/doc/UserGuide.md"
              "${CMAKE_SOURCE_DIR}/doc/PythonBindings.md"
              "${CMAKE_SOURCE_DIR}/doc/acrh-paper.pdf"
        DESTINATION "${docdest}")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/doc/example/"
        DESTINATION "${docdest}/example")
# headers
# this is kind of a hack. it would probably be best to somehow move the header installation
# to the subdirectory's CMakeLists.txt TODO(fpetran)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/src/"
        DESTINATION "${DESTINATION}/include/norma"
        FILES_MATCHING
            PATTERN "*.h"
            PATTERN "doc" EXCLUDE
            PATTERN "tests" EXCLUDE
            PATTERN "example" EXCLUDE
            PATTERN "scripts" EXCLUDE
            PATTERN "CMakeModules" EXCLUDE
            PATTERN "python" EXCLUDE
            PATTERN "data" EXCLUDE
            PATTERN ".git" EXCLUDE)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/src/defines.h"
        DESTINATION "${DESTINATION}/include/norma")

############################### docs ###########################################
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
                   @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation..."
    )
endif()
