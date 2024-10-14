set(INX_TEST_EXECUTABLE inx_test CACHE INTERNAL "")

add_executable(${INX_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${INX_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${INX_TEST_EXECUTABLE})