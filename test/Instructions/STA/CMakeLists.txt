set(STA_TEST_EXECUTABLE sta_test CACHE INTERNAL "")


add_executable(${STA_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${STA_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${STA_TEST_EXECUTABLE})