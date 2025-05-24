set(EXECUTABLE_NAME nes_emulator)

# Configure SFML for static linking
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(SFML_USE_STATIC_STD_LIBS TRUE CACHE BOOL "Use static C++ standard libraries" FORCE)
set(SFML_STATIC_LIBRARIES TRUE CACHE BOOL "Use static SFML libraries" FORCE)

CPMAddPackage(
  NAME SFML
  GITHUB_REPOSITORY SFML/SFML
  GIT_TAG 2.5.1
)

add_executable(${EXECUTABLE_NAME} main.cpp)

# When linking statically with SFML, you need to add -DSFML_STATIC to compilation flags
target_compile_definitions(${EXECUTABLE_NAME} PRIVATE SFML_STATIC)

# Link with static SFML libraries
target_link_libraries(${EXECUTABLE_NAME} PRIVATE 
  sfml-system
  sfml-window
  sfml-graphics
  # Add other components as needed: sfml-audio, sfml-network
)