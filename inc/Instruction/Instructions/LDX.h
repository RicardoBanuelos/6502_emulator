#pragma once

#include "Instruction/Instruction.h"

class LDX : public Instruction
{
    
public:
    LDX( std::shared_ptr<ICPU> icpu, 
         AddressingMode addressingMode, 
         uint8_t cycles = 0);

    ~LDX();

    void run() override;
private:
};

