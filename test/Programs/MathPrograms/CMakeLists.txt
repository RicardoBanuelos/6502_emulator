set(PROGRAMS_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(GENERATED_HEADERS_FOLDER ${CMAKE_BINARY_DIR}/generated/mathprograms)
set(PROGRAM_NAME math_programs_test)
set(CONFIG_FILE_PATH_IN test_paths.h.in)
set(CONFIG_FILE_PATH_OUT ${GENERATED_HEADERS_FOLDER}/test_paths.h)
set(CUSTOM_TARGET_NAME math_generate_test_paths)

set(MULTIPLY_TEST_PROGRAM "${PROGRAMS_RESOURCES_DIR}/multiplyProgram.bin")
set(BITWISE_AND_TEST_PROGRAM "${PROGRAMS_RESOURCES_DIR}/bitwiseAnd.bin")

# Move this before executable definition to ensure it runs first
add_custom_target(
    ${CUSTOM_TARGET_NAME} ALL
    DEPENDS ${CONFIG_FILE_PATH_OUT}
    COMMENT "Generating test_paths.h"
)

# Force the generation during configure time
configure_file(
  ${CONFIG_FILE_PATH_IN}
  ${CONFIG_FILE_PATH_OUT}
  IMMEDIATE  # This forces immediate generation
)

add_executable(${PROGRAM_NAME} test.cpp)

# Add explicit dependency
add_dependencies(${PROGRAM_NAME} ${CUSTOM_TARGET_NAME})

target_link_libraries(${PROGRAM_NAME}
  GTest::GTest
  ${CPU_LIBRARY_NAME}
)

target_include_directories(
  ${PROGRAM_NAME}
  PUBLIC
  ${GENERATED_HEADERS_FOLDER}
)

# Create compile_commands.json for better IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

gtest_discover_tests(${PROGRAM_NAME})