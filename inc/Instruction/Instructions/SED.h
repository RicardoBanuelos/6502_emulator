#pragma once 

#include "Instruction.h"

class SED : public Instruction
{

public:
    SED(std::shared_ptr<ICPU> icpu);
    ~SED();

    void run() override;
};
