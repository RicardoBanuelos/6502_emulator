#pragma once
#include "../Instruction.h"

class CLC : public Instruction 
{
public:
    CLC(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~CLC();

    void run() override;
};