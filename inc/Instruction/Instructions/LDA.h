#pragma once 

#include "Instruction/Instruction.h"

//Load Accumulator with Memory
class LDA : public Instruction
{
    
public:
    LDA(const std::string &name,
        std::shared_ptr<ICPU> icpu, 
        std::function<uint16_t()> addressingFunction, 
        uint8_t cycles = 0);

    ~LDA();

    void run() override;
private:
};
