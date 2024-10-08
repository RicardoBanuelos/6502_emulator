#pragma once

#include "../Instruction.h"

class BMI : public Instruction {
public:
    BMI(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~BMI();

    void run() override;
};