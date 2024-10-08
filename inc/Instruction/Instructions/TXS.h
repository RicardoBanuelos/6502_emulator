#pragma once

#include "Instruction/Instruction.h"

class TXS : public Instruction
{
    
public:
    TXS(std::shared_ptr<ICPU> icpu);
    ~TXS();

    void run() override;
private:
};

