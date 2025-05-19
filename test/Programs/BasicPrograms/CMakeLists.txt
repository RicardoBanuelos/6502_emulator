set(PROGRAMS_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(BASIC_PROGRAM_0_PATH "${PROGRAMS_RESOURCES_DIR}/basic_program_0.bin")
set(BASIC_PROGRAM_1_PATH "${PROGRAMS_RESOURCES_DIR}/basic_program_1.bin")
set(BASIC_PROGRAM_2_PATH "${PROGRAMS_RESOURCES_DIR}/basic_program_2.bin")
set(BASIC_PROGRAM_3_PATH "${PROGRAMS_RESOURCES_DIR}/basic_program_3.bin")
set(BASIC_MATH_PROGRAM_0_PATH "${PROGRAMS_RESOURCES_DIR}/basic_math_program_0.bin")
set(BASIC_PROGRAM_SQUARE_ROOT "${PROGRAMS_RESOURCES_DIR}/square_root.bin")


set(CONFIG_FILE_PATH_IN test_paths.h.in)
set(CONFIG_FILE_PATH_OUT ${CMAKE_BINARY_DIR}/generated/basicprograms/test_paths.h)

# Move this before executable definition to ensure it runs first
add_custom_target(
    generate_test_paths ALL
    DEPENDS ${CONFIG_FILE_PATH_OUT}
    COMMENT "Generating test_paths.h"
)

# Force the generation during configure time
configure_file(
  ${CONFIG_FILE_PATH_IN}
  ${CONFIG_FILE_PATH_OUT}
  IMMEDIATE  # This forces immediate generation
)

set(BASIC_PROGRAMS_TEST basic_programs_test)

add_executable(${BASIC_PROGRAMS_TEST} test.cpp)

# Add explicit dependency
add_dependencies(${BASIC_PROGRAMS_TEST} generate_test_paths)

target_link_libraries(${BASIC_PROGRAMS_TEST}
  GTest::GTest
  ${CPU_LIBRARY_NAME}
)

target_include_directories(
  ${BASIC_PROGRAMS_TEST}
  PUBLIC
  ${CMAKE_BINARY_DIR}/generated/basicprograms
)

# Create compile_commands.json for better IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

gtest_discover_tests(${BASIC_PROGRAMS_TEST})