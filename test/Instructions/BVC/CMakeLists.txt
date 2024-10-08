set(BVC_TEST_EXECUTABLE bvc_test CACHE INTERNAL "")

add_executable(${BVC_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BVC_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${BVC_TEST_EXECUTABLE})