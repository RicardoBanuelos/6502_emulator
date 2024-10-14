#pragma once

#include "Instruction/Instruction.h"

class LSR : public Instruction
{
    
public:
    LSR(    std::shared_ptr<ICPU> icpu, 
            AddressingMode addressingMode, 
            uint8_t cycles = 0);

    ~LSR();

    void run() override;

private:


};

