#pragma once 

#include "Instruction/Instruction.h"

class SBC : public Instruction
{
    
public:
    SBC(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode, 
        uint8_t cycles = 0);
    ~SBC();

    void run() override;
};
