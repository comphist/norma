add_sources(mapper.cpp result.cpp)
install_headers(base.h exceptions.h
                mapper.h result.h rulebased.h wld.h)
if (WITH_PYTHON)
    install_headers(external.h)
endif()
