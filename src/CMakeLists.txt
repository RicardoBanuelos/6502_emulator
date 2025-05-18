set(BUS_LIBRARY_NAME buslib CACHE INTERNAL "Bus library name")
set(MEMORY_LIBRARY_NAME memorylib CACHE INTERNAL "Memory library name")
set(ICPU_LIBRARY_NAME icpulib CACHE INTERNAL "ICPU library name")
set(CPU_LIBRARY_NAME cpulib CACHE INTERNAL "Bus library name")
set(INSTRUCTIONS_LIBRARY_NAME instructionslib CACHE INTERNAL "Instructions library name")
set(ADDRESSING_LIBRARY_NAME addressinglib CACHE INTERNAL "Addressing library name")

add_subdirectory(Memory)
add_subdirectory(Bus)
add_subdirectory(Instruction)
add_subdirectory(ICPU)
add_subdirectory(CPU)
add_subdirectory(Addressing)

