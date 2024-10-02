#pragma once
#include "../Instruction.h"

class CLI : public Instruction 
{
public:
    CLI(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~CLI();

    void run() override;
};