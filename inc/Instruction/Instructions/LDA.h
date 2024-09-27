#pragma once 

#include "Instruction/Instruction.h"

//Load Accumulator with Memory
class LDA : public Instruction
{
    
public:
    LDA(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode, 
        uint8_t cycles = 0);

    ~LDA();

    void run() override;
private:
};
