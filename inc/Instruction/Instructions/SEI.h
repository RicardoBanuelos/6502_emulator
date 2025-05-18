#pragma once

#include "Instruction.h"

class SEI : public Instruction
{
    
public:
    SEI(std::shared_ptr<ICPU> icpu);
    ~SEI();

    void run() override;
private:
};

