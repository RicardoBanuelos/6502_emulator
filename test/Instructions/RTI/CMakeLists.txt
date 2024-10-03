set(RTI_TEST_EXECUTABLE rti_test CACHE INTERNAL "")


add_executable(${RTI_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${RTI_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${RTI_TEST_EXECUTABLE})