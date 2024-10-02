#pragma once
#include "../Instruction.h"

class DEY : public Instruction 
{
public:
    DEY(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~DEY();

    void run() override;
};