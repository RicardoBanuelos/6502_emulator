#pragma once

#include "../Instruction.h"

class BPL : public Instruction {
public:
    BPL(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~BPL();

    void run() override;
};