#pragma once

#include "Instruction/Instruction.h"

class STY : public Instruction
{
    
public:
    STY(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode, 
        uint8_t cycles = 0);

    ~STY();
    void run() override;
    
private:


};

