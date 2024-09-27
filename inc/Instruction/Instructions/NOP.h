#pragma once

#include "Instruction/Instruction.h"

class NOP : public Instruction
{
    
public:
    NOP(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode, 
        uint8_t cycles = 0);
    
    ~NOP();
private:
};