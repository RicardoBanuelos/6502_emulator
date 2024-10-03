set(BVS_TEST_EXECUTABLE bvs_test CACHE INTERNAL "")

add_executable(${BVS_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${BVS_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${BVS_TEST_EXECUTABLE})