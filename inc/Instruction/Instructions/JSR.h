#pragma once


#include "Instruction/Instruction.h"

class JSR : public Instruction
{
    
public:
    JSR(std::shared_ptr<ICPU> icpu, 
        std::function<uint16_t()> addressingFunction, 
        uint8_t cycles = 0);
    ~JSR();

    void run() override;
private:

};

