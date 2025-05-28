#pragma once


#include "Instruction.h"

class JSR : public Instruction
{
    
public:
    JSR(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode = AddressingMode::Absolute, uint8_t cycleCount = 6);
    ~JSR();

    void run() override;
private:

};

