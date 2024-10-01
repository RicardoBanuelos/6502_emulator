set(CPU_TEST cpu_test CACHE INTERNAL "")


add_executable(${CPU_TEST} cpu_test.cpp)


target_link_libraries(${CPU_TEST}
  GTest::GTest
  ${CPU_LIBRARY}
  ${INTRUCTIONS_LIBRARY} 
  ${MEMORY_LIBRARY}
  ${BUS_LIBRARY}
  )

gtest_discover_tests(${CPU_TEST})