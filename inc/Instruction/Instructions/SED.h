#pragma once 

#include "Instruction/Instruction.h"

class SED : public Instruction
{

public:
    SED(std::shared_ptr<ICPU> icpu);
    ~SED();


    void run() override;
};
