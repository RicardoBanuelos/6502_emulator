#pragma once
#include "../Instruction.h"

class CMP : public Instruction 
{
public:
    CMP(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~CMP();

    void run() override;
};