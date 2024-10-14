#pragma once
#include "../Instruction.h"

class INY : public Instruction 
{
public:
    INY(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~INY();

    void run() override;
};