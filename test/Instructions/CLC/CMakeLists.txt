set(CLC_TEST_EXECUTABLE clc_test CACHE INTERNAL "")

add_executable(${CLC_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${CLC_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${CLC_TEST_EXECUTABLE})