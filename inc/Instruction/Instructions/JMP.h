#pragma once
#include "../Instruction.h"

class JMP : public Instruction 
{
public:
    JMP(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~JMP();

    void run() override;
};