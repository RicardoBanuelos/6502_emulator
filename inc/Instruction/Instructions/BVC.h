#pragma once
#include "../Instruction.h"

class BVC : public Instruction 
{
public:
    BVC(std::shared_ptr<ICPU> icpu);

    ~BVC();

    void run() override;
};