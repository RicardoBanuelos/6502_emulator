set(TYA_TEST_EXECUTABLE tya_test CACHE INTERNAL "")


add_executable(${TYA_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${TYA_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${TYA_TEST_EXECUTABLE})