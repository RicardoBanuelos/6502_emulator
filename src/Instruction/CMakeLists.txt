set(INTRUCTIONS_LIBRARY libInstructions CACHE INTERNAL "")

add_library(${INTRUCTIONS_LIBRARY} 
    Instruction.cpp
    Instructions/ADC.cpp
    Instructions/AND.cpp
    Instructions/ASL.cpp
    Instructions/BCC.cpp
    Instructions/BCS.cpp
    Instructions/BEQ.cpp
    Instructions/BIT.cpp
    Instructions/BMI.cpp
    Instructions/BNE.cpp
    Instructions/BPL.cpp
    Instructions/BRK.cpp
    Instructions/JSR.cpp
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


