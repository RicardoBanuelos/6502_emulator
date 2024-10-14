set(JSR_TEST_EXECUTABLE jsr_test CACHE INTERNAL "")


add_executable(${JSR_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${JSR_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${JSR_TEST_EXECUTABLE})