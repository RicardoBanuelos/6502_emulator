#pragma once

#include "../Instruction.h"

class BCC : public Instruction {
public:
    BCC(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~BCC();

    void run() override;
};