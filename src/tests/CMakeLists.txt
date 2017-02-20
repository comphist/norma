# this is a horrible, horrible hack: cmake doesn't allow adding dependencies
# to built-in targets like "test", but at the same time, add_test doesn't add
# a dependency on the executable for the test target. so the only way to have
# make test depend on the tests without having tests in all is to add a "test"
# that builds the tests and make all other tests depend on it.
# cf. CMake bugs #8774, #8438
# fpetran 2015/03/17

add_test(ctest_build_test_code "${CMAKE_COMMAND}" --build ${CMAKE_BINARY_DIR} --target buildtests)

macro(add_complete_test binary source testname)
    add_executable(${binary} ${source})
    add_dependencies(buildtests ${binary})
    target_link_libraries(${binary} norma ${Boost_UNIT_TEST_FRAMEWORK} ${ARGN})
    add_test(${testname} ${binary})
    set_tests_properties(${testname} PROPERTIES DEPENDS ctest_build_test_code)
endmacro()

add_complete_test(gfsm_wrapper gfsm_wrapper.cpp Gfsm ${LIBGFSM_LIBRARIES})
add_complete_test(training_data training_data.cpp TrainingData)
add_complete_test(interface interface_test.cpp Interface)
add_subdirectory(normalizer)

if(WITH_PYTHON)
  add_subdirectory(python)
endif()
