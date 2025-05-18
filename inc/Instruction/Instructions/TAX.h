#pragma once

#include "Instruction.h"

class TAX : public Instruction
{
    
public:
    TAX(std::shared_ptr<ICPU> icpu);
    ~TAX();

    void run() override;
private:
};

