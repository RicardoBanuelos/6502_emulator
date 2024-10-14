set(DEC_TEST_EXECUTABLE dec_test CACHE INTERNAL "")

add_executable(${DEC_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${DEC_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${DEC_TEST_EXECUTABLE})