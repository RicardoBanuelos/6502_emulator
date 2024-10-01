set(MEMORY_TEST memory_test)


add_executable(${MEMORY_TEST} Memory_Test.cpp)


target_link_libraries(${MEMORY_TEST}
  GTest::GTest
  ${MEMORY_LIBRARY})

gtest_discover_tests(${MEMORY_TEST})