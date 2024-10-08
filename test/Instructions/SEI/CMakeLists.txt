set(SEI_TEST_EXECUTABLE sei_test CACHE INTERNAL "")


add_executable(${SEI_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${SEI_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${SEI_TEST_EXECUTABLE})