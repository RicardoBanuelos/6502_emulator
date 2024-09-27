#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "CPU/ICPU.h"

class Instruction
{
    
public:
    Instruction(const std::string &name,
                std::shared_ptr<ICPU> icpu, 
                std::function<uint16_t()> addressingFunction, 
                uint8_t cycles = 0);

    virtual ~Instruction();
    virtual void run() = 0;

protected:
    std::string mName;
    std::function<uint16_t()> mAddressingFunction;
    std::shared_ptr<ICPU> mIcpu;
    uint8_t mCycles;
};