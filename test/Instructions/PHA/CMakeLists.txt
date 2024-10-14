set(PHA_TEST_EXECUTABLE pha_test CACHE INTERNAL "")


add_executable(${PHA_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${PHA_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${PHA_TEST_EXECUTABLE})