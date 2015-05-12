########################## python bindings #####################################
# need Boost::Python
find_package(Boost 1.50 COMPONENTS python REQUIRED)

add_library(norma-python MODULE
            string_impl_conv.cpp result_conv.cpp training_conv.cpp
            exception_wrapper.cpp lexicon_wrapper.cpp
            norma.cpp)
### "The name used in BOOST_PYTHON_MODULE must match the name of
### the .so library you generate and import into python."
### --see <http://stackoverflow.com/questions/8400244/boost-python-import-error-module-does-not-define-init-function>
set_target_properties(norma-python PROPERTIES PREFIX ""
                                              OUTPUT_NAME "norma"
                                              LIBRARY_OUTPUT_DIRECTORY "norma/")
set(NORMA_PYTHON_LIBRARIES norma ${PYTHON_LIBRARIES} ${Boost_PYTHON_LIBRARY})
target_link_libraries(norma-python LINK_PUBLIC ${NORMA_PYTHON_LIBRARIES})

### let python tell us what the appropriate directory for site packages is
execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print get_python_lib(prefix='${DESTINATION}')"
                        OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
set(NORMA_PYTHON_INSTALL_DIR ${PYTHON_SITE_PACKAGES}/norma)
install(TARGETS norma-python DESTINATION ${NORMA_PYTHON_INSTALL_DIR})

### setup.py isn't currently used, but let's leave it in here in case we ever
### decide to do this via setuptools
#configure_file(setup.py.in setup.py)

macro(add_normalizer_bindings _NORM_NAME)
    add_library("${_NORM_NAME}-python" MODULE normalizer/${_NORM_NAME}.cpp)
    set_target_properties("${_NORM_NAME}-python"
                          PROPERTIES PREFIX ""
                                     OUTPUT_NAME ${_NORM_NAME}
                                     LIBRARY_OUTPUT_DIRECTORY "norma/")
    target_link_libraries("${_NORM_NAME}-python" LINK_PUBLIC ${_NORM_NAME} ${NORMA_PYTHON_LIBRARIES})
    install(TARGETS "${_NORM_NAME}-python" DESTINATION ${NORMA_PYTHON_INSTALL_DIR})
endmacro(add_normalizer_bindings)

add_normalizer_bindings(rulebased)
add_normalizer_bindings(wld)
add_normalizer_bindings(mapper)

### native Python files that complement the bindings
add_subdirectory(norma)

