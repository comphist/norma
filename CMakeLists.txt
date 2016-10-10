cmake_minimum_required(VERSION 2.8.10)
project(norma)
# query git tags to set version
execute_process(
    COMMAND sh GIT-VERSION-GEN
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE CMAKE_PROJECT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

########## command line arguments {{{
set(STRING_IMPL "ICU" CACHE STRING "String implementation to use (ICU|STD)")
set(NORMA_DEFAULT_PLUGIN_BASE
    "${CMAKE_INSTALL_PREFIX}/share/norma/plugins" CACHE STRING
    "Default directory for normalizer plugins")
set(WITH_PYTHON OFF CACHE BOOL "Whether to make Python buildings/embed")
set(WITH_TAGS OFF CACHE BOOL "Whether to generate ctags/cscope in build")
set(WITH_LINT OFF CACHE BOOL "Whether to include a lint (code check) target")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")
if (WITH_LINT)
    include(cpplint)
endif()
if (WITH_TAGS)
    include(CTAGS)
endif()
enable_testing()
include(AddSources)
# }}}
########## find dependencies {{{
find_package(Boost 1.50
    COMPONENTS program_options unit_test_framework regex system filesystem
    REQUIRED)
if(NOT PKG_CONFIG STREQUAL "")
    set(PKG_CONFIG_EXECUTABLE ${PKG_CONFIG})
endif()
find_package(PkgConfig REQUIRED)
find_package(GLIB2 REQUIRED)
include_directories(${GLIB2_INCLUDE_DIR})
find_package(Gfsm REQUIRED)
include_directories(${LIBGFSM_INCLUDE_DIRS})
# }}}
########## string implementation {{{
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
# }}}
########## python bindings {{{
if(WITH_PYTHON)
    find_package(PythonLibs 2.7 REQUIRED)
    find_package(PythonInterp 2.7 REQUIRED)
    include_directories(${PYTHON_INCLUDE_DIRS})
endif()
# }}}
########## configure headers {{{
set( NORMA_TEST_BASE_DIR "${CMAKE_SOURCE_DIR}/src/tests/data" )
configure_file( "${CMAKE_SOURCE_DIR}/src/config.h.in"
                "${CMAKE_BINARY_DIR}/src/config.h" )
configure_file( "${CMAKE_SOURCE_DIR}/src/defines.h.in"
                "${CMAKE_BINARY_DIR}/src/defines.h")
include_directories( "${CMAKE_BINARY_DIR}/src" )
# }}}
########## compiler setting and flags {{{
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math")
    if(${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER "4.8.99")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    else()
        message(FATAL_ERROR "You need at least g++ >= 4.9! Your version is ${CMAKE_CXX_COMPILER_VERSION}")
    endif()
    set(CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -Og -ggdb")
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")
endif()
# }}}
########## main source directories {{{
add_subdirectory(src)
add_subdirectory(src/interface)
add_subdirectory(src/normalizer)
add_subdirectory(src/gfsm)
add_subdirectory(src/lexicon)
include_directories(src src/normalizer src/interface src/gfsm)
if(WITH_PYTHON)
    add_subdirectory(src/python)
endif()
# }}}
########## library and executables {{{
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
add_executable(normalize src/main.cpp)
target_link_libraries(normalize
                      norma
                      ${Boost_PROGRAM_OPTIONS_LIBRARY})
# }}}
########## tests {{{
add_custom_target(buildtests)
add_subdirectory(src/tests EXCLUDE_FROM_ALL)

if (CMAKE_CONFIGURATION_TYPES)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
        --force-new-ctest-process --output-on-failure
        --build-config "$<CONFIGURATION>")
else()
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
        --force-new-ctest-process --output-on-failure)
endif()
add_dependencies(check buildtests)
# }}}
########## install {{{
if (NOT CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")
endif()
if (NOT CMAKE_INSTALL_BINDIR)
    set(CMAKE_INSTALL_BINDIR "bin")
endif()
set(NORMA_LIBDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(NORMA_BINDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
set(NORMA_DOCDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/doc/${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}")
# library
install(TARGETS norma DESTINATION "${NORMA_LIBDIR}")
# binary
install(TARGETS normalize DESTINATION "${NORMA_BINDIR}")
# docs/examples
install(FILES "${CMAKE_SOURCE_DIR}/README.md"
              "${CMAKE_SOURCE_DIR}/doc/UserGuide.md"
              "${CMAKE_SOURCE_DIR}/doc/PythonBindings.md"
              "${CMAKE_SOURCE_DIR}/doc/acrh-paper.pdf"
              DESTINATION "${NORMA_DOCDIR}")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/doc/example/"
                  DESTINATION "${NORMA_DOCDIR}/example")
# headers are installed from the source directory they live in
# except for configured headers. config.h only contains
# build related stuff and doesn't need to be installed
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/src/defines.h"
    DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${CMAKE_PROJECT_NAME}")
# }}}
########## documentation {{{
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
# }}}
# vim: set fdm=marker fmr={,}: fdls=1
