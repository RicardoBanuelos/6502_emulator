set(BMI_TEST_EXECUTABLE bmi_test CACHE INTERNAL "")


add_executable(${BMI_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BMI_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${BMI_TEST_EXECUTABLE})