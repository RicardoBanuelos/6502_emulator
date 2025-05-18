set(BUS_LIBRARY_NAME buslib)
set(MEMORY_LIBRARY_NAME memorylib)
set(ICPU_LIBRARY_NAME icpulib)
set(CPU_LIBRARY_NAME cpulib)
set(INSTRUCTIONS_LIBRARY_NAME instructionslib)
set(ADDRESSING_LIBRARY_NAME addressinglib)

add_subdirectory(Memory)
add_subdirectory(Bus)
add_subdirectory(Instruction)
add_subdirectory(ICPU)
add_subdirectory(CPU)
add_subdirectory(Addressing)

