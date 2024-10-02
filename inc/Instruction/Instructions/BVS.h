#pragma once
#include "../Instruction.h"

class BVS : public Instruction 
{
public:
    BVS(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~BVS();

    void run() override;
};