#pragma once

#include "Instruction/Instruction.h"

class ROL : public Instruction
{
    
public:
    ROL(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode, 
        uint8_t cycles = 0);
    
    ~ROL();
    void run() override;
private:
};
