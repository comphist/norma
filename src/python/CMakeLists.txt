########################## python bindings #####################################
# need Boost::Python
find_package(Boost 1.50 COMPONENTS python REQUIRED)

# add library
set(PY_SRCS
    string_impl_conv.cpp result_conv.cpp training_conv.cpp
    exception_wrapper.cpp lexicon_wrapper.cpp normalizer_wrapper.cpp
    norma.cpp)

add_library(norma-python MODULE ${PY_SRCS})
### "The name used in BOOST_PYTHON_MODULE must match the name of
### the .so library you generate and import into python."
### --see <http://stackoverflow.com/questions/8400244/boost-python-import-error-module-does-not-define-init-function>
set_target_properties(norma-python PROPERTIES PREFIX ""
                                              OUTPUT_NAME "norma"
                                              LIBRARY_OUTPUT_DIRECTORY "norma/")

target_link_libraries(norma-python
  LINK_PUBLIC
  norma
  ${PYTHON_LIBRARIES}
  ${Boost_PYTHON_LIBRARY}
)

### let python tell us what the appropriate directory for site packages is
execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print get_python_lib(prefix='${DESTINATION}')" OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
set(NORMA_PYTHON_INSTALL_DIR ${PYTHON_SITE_PACKAGES}/norma)
install(TARGETS norma-python DESTINATION ${NORMA_PYTHON_INSTALL_DIR})

### setup.py isn't currently used, but let's leave it in here in case we ever
### decide to do this via setuptools
#configure_file(setup.py.in setup.py)

### native Python files that complement the bindings
add_subdirectory(norma)
