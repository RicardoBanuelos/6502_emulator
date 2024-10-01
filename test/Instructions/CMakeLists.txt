set(LDA_TEST_EXECUTABLE lda_test CACHE INTERNAL "")


add_executable(${LDA_TEST_EXECUTABLE} LDA_test.cpp)


target_link_libraries(${LDA_TEST_EXECUTABLE}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY})

gtest_discover_tests(${LDA_TEST_EXECUTABLE})