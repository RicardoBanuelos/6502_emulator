set(BCC_TEST_EXECUTABLE bcc_test CACHE INTERNAL "")


add_executable(${BCC_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BCC_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${BCC_TEST_EXECUTABLE})