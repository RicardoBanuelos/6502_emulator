set(MEMORY_TEST memory_test)
set(MEM_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(MEMORY_BINARY_PATH "${MEM_RESOURCES_DIR}/memtest.bin")


set(CONFIG_FILE_PATH_IN test_paths.h.in)
set(CONFIG_FILE_PATH_OUT ${CMAKE_BINARY_DIR}/generated/memory/test_paths.h)
# Move this before executable definition to ensure it runs first
add_custom_target(
    generate_test_paths_memory ALL
    DEPENDS ${CONFIG_FILE_PATH_OUT}
    COMMENT "Generating test_paths.h"
)

# Force the generation during configure time
configure_file(
  ${CONFIG_FILE_PATH_IN}
  ${CONFIG_FILE_PATH_OUT}
  IMMEDIATE  # This forces immediate generation
)



add_executable(${MEMORY_TEST} Memory_Test.cpp)
add_dependencies(${MEMORY_TEST} generate_test_paths)


target_link_libraries(${MEMORY_TEST}
  GTest::GTest
  ${MEMORY_LIBRARY_NAME}
)

target_include_directories(
  ${MEMORY_TEST}
  PUBLIC
  ${CMAKE_BINARY_DIR}/generated/memory
)



gtest_discover_tests(${MEMORY_TEST})