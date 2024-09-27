#pragma once


#include "Instruction/Instruction.h"

class JSR : public Instruction
{
    
public:
    JSR(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode, 
        uint8_t cycles = 0);
    ~JSR();

    void run() override;
private:

};

