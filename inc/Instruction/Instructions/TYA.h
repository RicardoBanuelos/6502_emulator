#pragma once

#include "Instruction/Instruction.h"

class TYA : public Instruction
{
    
public:
    TYA(std::shared_ptr<ICPU> icpu);
    ~TYA();

    void run() override;
private:
};

