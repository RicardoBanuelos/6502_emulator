add_subdirectory(CPU)
add_subdirectory(Instruction)
add_subdirectory(Memory)
add_subdirectory(Bus)


add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${CPU_LIBRARY} ${INTRUCTIONS_LIBRARY} ${MEMORY_LIBRARY})