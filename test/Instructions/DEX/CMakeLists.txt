set(DEX_TEST_EXECUTABLE dex_test CACHE INTERNAL "")

add_executable(${DEX_TEST_EXECUTABLE} test.cpp)

target_link_libraries(${DEX_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY})

gtest_discover_tests(${DEX_TEST_EXECUTABLE})