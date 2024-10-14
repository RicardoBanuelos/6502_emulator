#pragma once


#include "Instruction/Instruction.h"

class JSR : public Instruction
{
    
public:
    JSR(std::shared_ptr<ICPU> icpu);
    ~JSR();

    void run() override;
private:

};

