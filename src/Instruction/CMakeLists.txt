set(INTRUCTIONS_LIBRARY libInstructions CACHE INTERNAL "")

add_library(${INTRUCTIONS_LIBRARY} 
    Instruction.cpp
    Instructions/LDA.cpp
    Instructions/JSR.cpp
    Instructions/LDX.cpp
    Instructions/LDY.cpp
    Instructions/LSR.cpp
    Instructions/NOP.cpp
    Instructions/ORA.cpp
    Instructions/PHA.cpp
    Instructions/PHP.cpp
    Instructions/PLA.cpp
    Instructions/PLP.cpp
    Instructions/ROL.cpp
    Instructions/ROR.cpp
    Instructions/RTI.cpp
    Instructions/RTS.cpp
    Instructions/SBC.cpp
)