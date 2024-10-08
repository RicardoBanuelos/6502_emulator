set(ROL_TEST_EXECUTABLE rol_test CACHE INTERNAL "")


add_executable(${ROL_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${ROL_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${ROL_TEST_EXECUTABLE})