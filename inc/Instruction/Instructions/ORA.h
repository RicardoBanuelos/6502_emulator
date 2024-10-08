#pragma once
#include "Instruction/Instruction.h"

class ORA : public Instruction
{
    
public:
    ORA(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode, 
        uint8_t cycles = 0);
    ~ORA();


    void run() override;
private:
};
