#pragma once 

#include "Instruction/Instruction.h"

class SEC : public Instruction
{
    
public:
    SEC(std::shared_ptr<ICPU> icpu);
    ~SEC();

    void run() override;
};

