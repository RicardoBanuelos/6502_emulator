#pragma once
#include "../Instruction.h"

class CLD : public Instruction 
{
public:
    CLD(std::shared_ptr<ICPU> icpu);

    ~CLD();

    void run() override;
};