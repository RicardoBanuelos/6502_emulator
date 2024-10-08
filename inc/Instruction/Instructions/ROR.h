#pragma once

#include "Instruction/Instruction.h"

class ROR : public Instruction
{
    
public:
    ROR(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode, 
        uint8_t cycles = 0);
    ~ROR();

    void run() override;
};

