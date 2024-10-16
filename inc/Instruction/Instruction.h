#pragma once

#include <functional>
#include <memory>
#include <string>
#include "CPU/ICPU.h"
#include "CPU/Addressing.h"

class Instruction
{
    
public:
    Instruction(const std::string &name,
                std::shared_ptr<ICPU> icpu, 
                AddressingMode addressingMode, 
                uint8_t cycles = 0);

    virtual ~Instruction();
    virtual void run() = 0;
    AddressingMode mode() const;
    uint8_t cycles() const;
    const std::string &name() const;
protected:
    std::string mName;
    AddressingMode mAddressingMode;
    std::shared_ptr<ICPU> mIcpu;
    uint8_t mCycles;
};
