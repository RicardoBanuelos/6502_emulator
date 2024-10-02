#pragma once
#include "../Instruction.h"

class CLV : public Instruction 
{
public:
    CLV(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~CLV();

    void run() override;
};