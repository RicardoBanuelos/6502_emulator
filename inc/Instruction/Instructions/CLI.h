#pragma once
#include "../Instruction.h"

class CLI : public Instruction 
{
public:
    CLI(std::shared_ptr<ICPU> icpu);

    ~CLI();

    void run() override;
};