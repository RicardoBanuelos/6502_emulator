set(ORA_TEST_EXECUTABLE ora_test CACHE INTERNAL "")


add_executable(${ORA_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${ORA_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${ORA_TEST_EXECUTABLE})