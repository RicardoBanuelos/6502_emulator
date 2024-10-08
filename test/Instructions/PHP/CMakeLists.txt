set(PHP_TEST_EXECUTABLE php_test CACHE INTERNAL "")


add_executable(${PHP_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${PHP_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${PHP_TEST_EXECUTABLE})