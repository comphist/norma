set(PY_NATIVE_FILES
    __init__.py LexiconWrapper.py ChainNormalizer.py NormalizerWrapper.py
    NormaCfgParser.py)

foreach(PY_NATIVE_FILE ${PY_NATIVE_FILES})
  configure_file(${PY_NATIVE_FILE} ${PY_NATIVE_FILE} COPYONLY)
endforeach(PY_NATIVE_FILE)

install(FILES ${PY_NATIVE_FILES} DESTINATION ${NORMA_PYTHON_INSTALL_DIR})
