set(ADC_TEST_EXECUTABLE adc_test CACHE INTERNAL "")


add_executable(${ADC_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${ADC_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${ADC_TEST_EXECUTABLE})