#pragma once

#include "../Instruction.h"

class BCS : public Instruction {
public:
    BCS(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~BCS();

    void run() override;
};