# common headers
install_headers(result.h exceptions.h base.h)
# convenience headers
install_headers(exceptions.h rulebased.h wld.h mapper.h)

add_subdirectory(mapper)
add_subdirectory(wld)
add_subdirectory(rulebased)

if (WITH_PYTHON)
    add_subdirectory(external)
    install_headers(external.h)
endif()

set(NORMALIZER_LIBRARIES ${NORMALIZER_LIBRARIES} PARENT_SCOPE)

