#pragma once

#include <memory>
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
    AbsoluteYOffset,
    Indirect,
    IndirectX,
    IndiectY
};

class Addressing
{

public:
    Addressing(std::shared_ptr<ICPU> icpu);
    ~Addressing();

    virtual uint8_t Implied() const;
    virtual uint8_t Immediate() const;
    virtual uint8_t ZeroPage() const;
    virtual uint8_t ZeroPageX() const;
    virtual uint8_t ZeroPageY() const;
    virtual uint8_t Relative() const;
    virtual uint16_t Absolute() const;
    virtual uint16_t AbsoluteOffsetX() const;
    virtual uint16_t AbsoluteOffsetY() const;
    virtual uint16_t Indirect() const;
    virtual uint16_t IndirectX() const;
    virtual uint16_t IndirectY() const;
    

private:
    std::shared_ptr<ICPU> mIcpu;
};