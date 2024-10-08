#pragma once
#include "../Instruction.h"

class CPY : public Instruction 
{
public:
    CPY(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~CPY();

    void run() override;
};