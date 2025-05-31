set(COMBINED_INSTRUCTIONS combinedinstructions)

add_executable(${COMBINED_INSTRUCTIONS} main_test.cpp)
target_link_libraries(${COMBINED_INSTRUCTIONS}
  GTest::GTest
  ${CPU_LIBRARY_NAME}
)

# target_compile_definitions(${COMBINED_INSTRUCTIONS} PRIVATE UNIT_TEST)
gtest_discover_tests(${COMBINED_INSTRUCTIONS})

