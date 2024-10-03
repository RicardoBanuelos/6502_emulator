set(SEC_TEST_EXECUTABLE sec_test CACHE INTERNAL "")


add_executable(${SEC_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${SEC_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${SEC_TEST_EXECUTABLE})