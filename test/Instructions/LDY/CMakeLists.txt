set(LDY_TEST_EXECUTABLE ldy_test CACHE INTERNAL "")


add_executable(${LDY_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${LDY_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${LDY_TEST_EXECUTABLE})