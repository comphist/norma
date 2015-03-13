macro(add_complete_test binary source testname)
    add_executable(${binary} ${source})
    target_link_libraries(${binary} norma ${Boost_UNIT_TEST_FRAMEWORK} ${ARGN})
    add_test(${testname} ${binary})
endmacro()

add_complete_test(gfsm_wrapper gfsm_wrapper.cpp Gfsm ${LIBGFSM_LIBRARIES})
add_complete_test(training_data training_data.cpp TrainingData)
add_subdirectory(normalizer)

if(USE_PYTHON)
  add_subdirectory(python)
endif()
