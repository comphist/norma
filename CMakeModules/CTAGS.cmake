# CTAGS/CSCOPE
# 2013-2014 Florian Petran
# add a tags target that generates ctags/cscope and is also triggered by
# all target
# currently assumes *nix system, and ctags/cscope are in path but failure
# is non fatal
find_program(CTAGS ctags)
find_program(CSCOPE cscope)
if(NOT CTAGS STREQUAL "CTAGS-NOTFOUND")
    add_custom_target(ctags
        COMMAND ${CTAGS} --fields=+iaS --extra=+q -R ${CMAKE_SOURCE_DIR}/
        COMMAND cp tags ${CMAKE_SOURCE_DIR}/
    )
else()
    message(STATUS "ctags not found, skipping ctags generation.")
    set(CTAGS false)
endif()
if(NOT CSCOPE STREQUAL "CSCOPE-NOTFOUND")
    file(GLOB_RECURSE ALL_FILES ${CMAKE_SOURCE_DIR} *.cpp *.h)
    add_custom_target(cscope
        COMMAND ${CSCOPE} -bR ${ALLFILES}
        COMMAND cp cscope.out ${CMAKE_SOURCE_DIR}/
    )
else()
    message(STATUS "cscope not found, skipping cscope generation.")
    set(CSCOPE falsE)
endif()
if(CTAGS AND CSCOPE)
    add_custom_target(tags ALL
        COMMAND make ctags
        COMMAND make cscope
        COMMENT "Generating ctags..."
    )
endif()

