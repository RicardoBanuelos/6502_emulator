set(TAX_TEST_EXECUTABLE tax_test CACHE INTERNAL "")


add_executable(${TAX_TEST_EXECUTABLE} test.cpp)


target_link_libraries(${TAX_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${TAX_TEST_EXECUTABLE})