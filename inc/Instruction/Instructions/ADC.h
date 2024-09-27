#pragma once
#include "../Instruction.h"

class ADC : public Instruction 
{
public:
    ADC(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~ADC();

    void run() override;
};