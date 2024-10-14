#pragma once

#include "Instruction/Instruction.h"

class PLA : public Instruction
{
    
public:
    PLA(std::shared_ptr<ICPU> icpu, uint8_t cycles = 0);
    ~PLA();

    void run() override;
private:
};

