set(INY_TEST_EXECUTABLE iny_test CACHE INTERNAL "")

add_executable(${INY_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${INY_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${INY_TEST_EXECUTABLE})