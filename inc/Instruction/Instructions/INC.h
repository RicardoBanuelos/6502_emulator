#pragma once
#include "../Instruction.h"

class INC : public Instruction 
{
public:
    INC(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~INC();

    void run() override;
};