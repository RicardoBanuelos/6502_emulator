#pragma once

#include "Instruction.h"

class TSX : public Instruction
{
    
public:
    TSX(std::shared_ptr<ICPU> icpu);
    ~TSX();

    void run() override;
private:
};

