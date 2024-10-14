#pragma once

#include "../Instruction.h"

class BRK : public Instruction {
public:
    BRK(std::shared_ptr<ICPU> icpu);

    ~BRK();

    void run() override;
};