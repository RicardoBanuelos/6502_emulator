#pragma once
#include "../Instruction.h"

class BVC : public Instruction 
{
public:
    BVC(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~BVC();

    void run() override;
};