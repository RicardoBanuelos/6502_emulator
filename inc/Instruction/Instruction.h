#pragma once

#include "CPU/ICPU.h"

class Instruction
{
    
public:
    Instruction(ICPU *mIcpu);
    virtual ~Instruction();

    virtual void run() = 0;

protected:
    
    ICPU *mIcpu;
};
