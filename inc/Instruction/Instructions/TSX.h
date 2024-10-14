#pragma once

#include "Instruction/Instruction.h"

class TSX : public Instruction
{
    
public:
    TSX(std::shared_ptr<ICPU> icpu);
    ~TSX();

    void run() override;
private:
};

