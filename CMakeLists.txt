cmake_minimum_required(VERSION 3.5.0)
project(6502Emulator VERSION 0.1.0 LANGUAGES C CXX)

include_directories(inc)
add_executable(
    ${PROJECT_NAME} 
    src/main.cpp 
    src/CPU/CPU.cpp
    src/CPU/Addressing.cpp
    src/Instruction/Instruction.cpp
    src/Instruction/Instructions/ADC.cpp
    src/Instruction/Instructions/AND.cpp
    src/Instruction/Instructions/ASL.cpp
    src/Instruction/Instructions/BCC.cpp
    src/Instruction/Instructions/BCS.cpp
    src/Instruction/Instructions/BEQ.cpp
    src/Instruction/Instructions/BIT.cpp
    src/Instruction/Instructions/BMI.cpp
    src/Instruction/Instructions/BNE.cpp
    src/Instruction/Instructions/BPL.cpp
    src/Instruction/Instructions/LDA.cpp
    src/Instruction/Instructions/JSR.cpp
    src/Instruction/Instructions/LDX.cpp
    src/Instruction/Instructions/LDY.cpp
    src/Memory/Memory.cpp
)
