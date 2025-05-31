#pragma once
#include "Instruction.h"

class BVS : public Instruction 
{
public:
    BVS(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode = AddressingMode::Relative, uint8_t cycles = 2);
    ~BVS();

    void run() override;
};