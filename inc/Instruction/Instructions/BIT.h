#pragma once

#include "../Instruction.h"

class BIT : public Instruction {
public:
    BIT(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~BIT();

    void run() override;
};