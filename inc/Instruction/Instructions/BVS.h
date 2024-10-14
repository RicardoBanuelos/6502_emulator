#pragma once
#include "../Instruction.h"

class BVS : public Instruction 
{
public:
    BVS(std::shared_ptr<ICPU> icpu);
    ~BVS();

    void run() override;
};