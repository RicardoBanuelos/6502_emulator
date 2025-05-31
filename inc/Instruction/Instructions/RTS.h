#pragma once

#include "Instruction.h"

class RTS : public Instruction
{
    
public:
    RTS(std::shared_ptr<ICPU> icpu);
    ~RTS();

    void run() override;
};

