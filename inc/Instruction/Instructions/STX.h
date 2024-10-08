#pragma once

#include "Instruction/Instruction.h"

class STX : public Instruction
{
    
public:
    STX(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode, 
        uint8_t cycles = 0);

    ~STX();
    void run() override;
    
private:


};

