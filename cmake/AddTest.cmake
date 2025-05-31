function(add_gtest test_name sources libs)
  message(STATUS "Configuring ${test_name} Test")

  add_executable(${test_name} ${sources})
  target_link_libraries(${test_name}
    GTest::GTest
    ${libs}
  )

  gtest_discover_tests(${test_name})
endfunction(add_gtest)
