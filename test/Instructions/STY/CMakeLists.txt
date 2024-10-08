set(STY_TEST_EXECUTABLE sty_test CACHE INTERNAL "")


add_executable(${STY_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${STY_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${STY_TEST_EXECUTABLE})