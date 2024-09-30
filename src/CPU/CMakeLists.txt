set(CPU_LIBRARY cpulib CACHE INTERNAL "")
 

add_library(${CPU_LIBRARY} Addressing.cpp CPU.cpp)
