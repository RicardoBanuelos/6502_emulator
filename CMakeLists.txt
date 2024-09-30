cmake_minimum_required(VERSION 3.5.0)
project(6502Emulator VERSION 0.1.0 LANGUAGES C CXX)

include_directories(inc)
add_executable(
    ${PROJECT_NAME} 
    src/main.cpp 
    src/CPU/CPU.cpp
    src/CPU/Addressing.cpp
    src/Instruction/Instruction.cpp
    src/Instruction/Instructions/LDA.cpp
    src/Instruction/Instructions/JSR.cpp
    src/Instruction/Instructions/LDX.cpp
    src/Instruction/Instructions/LDY.cpp
    src/Instruction/Instructions/LSR.cpp
    src/Instruction/Instructions/NOP.cpp
    src/Instruction/Instructions/ORA.cpp
    src/Instruction/Instructions/PHA.cpp
    src/Instruction/Instructions/PHP.cpp
    src/Instruction/Instructions/PLA.cpp
    src/Instruction/Instructions/PLP.cpp
    src/Instruction/Instructions/ROL.cpp
    src/Instruction/Instructions/ROR.cpp
    src/Instruction/Instructions/RTI.cpp
    src/Instruction/Instructions/RTS.cpp
    src/Memory/Memory.cpp
)
