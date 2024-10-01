#pragma once

#include "Instruction/Instruction.h"

class PLP : public Instruction
{
    
public:
    PLP(std::shared_ptr<ICPU> icpu, uint8_t cycles = 0);
    ~PLP();

    void run() override;
private:
};

