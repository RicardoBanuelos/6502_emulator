#pragma once

#include "Instruction.h"

class TXA : public Instruction
{
    
public:
    TXA(std::shared_ptr<ICPU> icpu);
    ~TXA();

    void run() override;
private:
};

