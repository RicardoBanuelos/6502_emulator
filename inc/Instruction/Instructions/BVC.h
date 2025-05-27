#pragma once
#include "Instruction.h"

class BVC : public Instruction 
{
public:
    BVC(std::shared_ptr<ICPU> icpu,
                AddressingMode addressingMode = AddressingMode::Relative, 
                uint8_t cycles = 2);

    ~BVC();

    void run() override;
};