#pragma once
#include "../Instruction.h"

class INX : public Instruction 
{
public:
    INX(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~INX();

    void run() override;
};