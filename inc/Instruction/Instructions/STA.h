#pragma once

#include "Instruction/Instruction.h"

class STA : public Instruction
{
    
public:
    STA(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode, 
        uint8_t cycles = 0);
    ~STA();

    void run() override;
private:
};

