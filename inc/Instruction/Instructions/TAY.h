#pragma once

#include "Instruction.h"

class TAY : public Instruction
{
    
public:
    TAY(std::shared_ptr<ICPU> icpu);
    ~TAY();

    void run() override;
private:
};

