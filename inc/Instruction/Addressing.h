#pragma once

#include <memory>
#include <functional>
#include "CPU/ICPU.h"

enum AddressingMode
{
    Implied,
    Immediate,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Relative,
    Absolute,
    AbsoluteOffsetX,
    AbsoluteOffsetY,
    Indirect,
    IndirectX,
    IndirectY
};

class Addressing
{

public:
    Addressing(std::shared_ptr<ICPU> icpu);
    ~Addressing();

    virtual uint16_t Implied() const;
    virtual uint16_t Immediate() const;
    virtual uint16_t ZeroPage() const;
    virtual uint16_t ZeroPageX() const;
    virtual uint16_t ZeroPageY() const;
    virtual uint16_t Relative() const;
    virtual uint16_t Absolute() const;
    virtual uint16_t AbsoluteOffsetX() const;
    virtual uint16_t AbsoluteOffsetY() const;
    virtual uint16_t Indirect() const;
    virtual uint16_t IndirectX() const;
    virtual uint16_t IndirectY() const;
    

    std::function<uint16_t()> createFunctionBind(AddressingMode mode);
private:
    std::shared_ptr<ICPU> mIcpu;
};