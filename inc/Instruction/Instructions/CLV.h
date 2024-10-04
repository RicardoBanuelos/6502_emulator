#pragma once
#include "../Instruction.h"

class CLV : public Instruction 
{
public:
    CLV(std::shared_ptr<ICPU> icpu);

    ~CLV();

    void run() override;
};