#pragma once
#include "../Instruction.h"

class CLC : public Instruction 
{
public:
    CLC(std::shared_ptr<ICPU> icpu);

    ~CLC();

    void run() override;
};