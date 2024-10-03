#pragma once

#include "Instruction/Instruction.h"

class NOP : public Instruction
{
    
public:
    NOP(std::shared_ptr<ICPU> icpu);
    
    ~NOP();
    void run() override;
private:
};