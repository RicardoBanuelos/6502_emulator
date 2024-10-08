#pragma once

#include "../Instruction.h"

class BEQ : public Instruction {
public:
    BEQ(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~BEQ();

    void run() override;
};