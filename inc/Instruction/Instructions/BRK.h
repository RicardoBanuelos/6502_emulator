#pragma once

#include "../Instruction.h"

class BRK : public Instruction {
public:
    BRK(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~BRK();

    void run() override;
};