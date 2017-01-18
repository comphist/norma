# cpplint module
# 2013-2014 Florian Petran
# adds a custom lint target for style checking
# also tries to download cpplint.py if it is not found
# dependencies:
# - needs python (for running cpplint.py)
# - needs curl (for download)
# downloading also assumes *nix system (chmod) but failure is non fatal

find_program(CPPLINT cpplint.py)
find_program(PYTHON python)
if(CPPLINT STREQUAL "CPPLINT-NOTFOUND")
    find_package(CURL)
    if(CURL_FOUND)
        exec_program(curl
            ARGS "https://raw.githubusercontent.com/google/styleguide/gh-pages/cpplint/cpplint.py -O")
        exec_program(chmod
            ARGS "+x ${CMAKE_CURRENT_BINARY_DIR}/cpplint.py")
        set(CPPLINT "${CMAKE_CURRENT_BINARY_DIR}/cpplint.py")
    else()
        set(CPPLINT "CPPLINT-NOTFOUND")
    endif()
endif()
if(NOT CPPLINT STREQUAL "CPPLINT-NOTFOUND" AND NOT PYTHON STREQUAL "PYTHON-NOTFOUND")
    set(LINT_FILTER "--filter=-readability/streams,-build/header_guard,-build/c++11,-build/include")
    file(GLOB_RECURSE ALLFILES ${CMAKE_SOURCE_DIR} *.cpp *.h)
    add_custom_target(lint
        COMMENT "Checking C++ style conformity..."
        COMMAND ${PYTHON} ${CPPLINT} ${LINT_FILTER} ${ALLFILES}
    )
else()
    message(STATUS "C++ style checker not found, skipping lint target.")
endif()
