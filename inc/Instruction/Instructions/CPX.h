#pragma once
#include "../Instruction.h"

class CPX : public Instruction 
{
public:
    CPX(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~CPX();

    void run() override;
};