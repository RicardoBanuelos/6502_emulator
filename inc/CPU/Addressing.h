#pragma once

#include <memory>
#include <functional>
#include "CPU/ICPU.h"
#include "AddressingMode.h"
#include "AddressingData.h"

class Addressing
{

public:
    Addressing(std::shared_ptr<ICPU> icpu);
    ~Addressing();

    const std::function<AddressingData()> &addressingFunction(AddressingMode mode);
    
private:
    virtual AddressingData Implied() const;
    virtual AddressingData Accumulator() const;
    virtual AddressingData Immediate() const;
    virtual AddressingData ZeroPage() const;
    virtual AddressingData ZeroPageX() const;
    virtual AddressingData ZeroPageY() const;
    virtual AddressingData Relative() const;
    virtual AddressingData Absolute() const;
    virtual AddressingData AbsoluteOffsetX() const;
    virtual AddressingData AbsoluteOffsetY() const;
    virtual AddressingData Indirect() const;
    virtual AddressingData IndirectX() const;
    virtual AddressingData IndirectY() const;
    std::shared_ptr<ICPU> mIcpu;
};