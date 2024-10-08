#pragma once 

#include "Instruction/Instruction.h"

class LDY : public Instruction
{
    
public:
    LDY(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode, 
        uint8_t cycles = 0);

    ~LDY();

    void run() override;

private:


};
