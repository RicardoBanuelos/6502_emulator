#pragma once
#include "../Instruction.h"

class DEC : public Instruction 
{
public:
    DEC(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~DEC();

    void run() override;
};