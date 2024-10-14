set(CLV_TEST_EXECUTABLE clv_test CACHE INTERNAL "")

add_executable(${CLV_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${CLV_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${CLV_TEST_EXECUTABLE})