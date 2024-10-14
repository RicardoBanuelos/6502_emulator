#pragma once
#include "../Instruction.h"

class EOR : public Instruction 
{
public:
    EOR(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~EOR();

    void run() override;
};