set(SED_TEST_EXECUTABLE sed_test CACHE INTERNAL "")


add_executable(${SED_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${SED_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${SED_TEST_EXECUTABLE})