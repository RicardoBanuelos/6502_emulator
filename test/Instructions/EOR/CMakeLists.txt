set(EOR_TEST_EXECUTABLE eor_test CACHE INTERNAL "")

add_executable(${EOR_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${EOR_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${EOR_TEST_EXECUTABLE})