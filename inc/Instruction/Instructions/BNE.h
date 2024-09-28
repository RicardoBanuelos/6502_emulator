#pragma once

#include "../Instruction.h"

class BNE : public Instruction {
public:
    BNE(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~BNE();

    void run() override;
};