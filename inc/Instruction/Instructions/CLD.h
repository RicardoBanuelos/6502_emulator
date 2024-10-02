#pragma once
#include "../Instruction.h"

class CLD : public Instruction 
{
public:
    CLD(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~CLD();

    void run() override;
};