#pragma once

#include "Instruction/Instruction.h"

class RTS : public Instruction
{
    
public:
    RTS(std::shared_ptr<ICPU> icpu);
    ~RTS();

    void run() override;
};

