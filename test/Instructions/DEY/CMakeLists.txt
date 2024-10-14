set(DEY_TEST_EXECUTABLE dey_test CACHE INTERNAL "")

add_executable(${DEY_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${DEY_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${DEY_TEST_EXECUTABLE})