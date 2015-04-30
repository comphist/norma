add_sources(mapper.cpp lexicon.cpp result.cpp)
install_headers(base.h exceptions.h lexicon.h lexicon_interface.h
                mapper.h result.h rulebased.h wld.h)
if (WITH_PYTHON)
    install_headers(external.h)
endif()
