#pragma once
#include "../Instruction.h"

class DEX : public Instruction 
{
public:
    DEX(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~DEX();

    void run() override;
};