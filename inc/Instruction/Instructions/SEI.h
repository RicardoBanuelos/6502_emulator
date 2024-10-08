#pragma once

#include "Instruction/Instruction.h"

class SEI : public Instruction
{
    
public:
    SEI(std::shared_ptr<ICPU> icpu);
    ~SEI();

    void run() override;
private:
};

