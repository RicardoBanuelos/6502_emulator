#include "CPU/AddressingMode.h"


Addressing::Addressing(std::shared_ptr<ICPU> icpu)
    :   mIcpu(icpu)
{
}

Addressing::~Addressing()
{
}

uint16_t Addressing::Implied() const
{
    return 0;
}

uint16_t Addressing::Immediate() const
{
    return mIcpu->fetchByte() & 0x00FF;
}

uint16_t Addressing::ZeroPage() const
{
    return mIcpu->fetchByte() & 0x00FF;
}

uint16_t Addressing::ZeroPageX() const
{
    return (mIcpu->fetchByte() + mIcpu->registers().X) & 0x00FF;
}

uint16_t Addressing::ZeroPageY() const
{
    return (mIcpu->fetchByte() + mIcpu->registers().Y) & 0x00FF;
}

uint16_t Addressing::Relative() const
{
    uint8_t relativeAddress = mIcpu->fetchByte();
    if(relativeAddress & 0x80)
    {
        relativeAddress |= 0xFF00;
    }

    return relativeAddress & 0x00FF;
}

uint16_t Addressing::Absolute() const
{
    return mIcpu->fetchWord();
}

uint16_t Addressing::AbsoluteOffsetX() const
{
    return mIcpu->fetchWord() + mIcpu->registers().X;
}

uint16_t Addressing::AbsoluteOffsetY() const
{
    return mIcpu->fetchWord() + mIcpu->registers().Y;
}


uint16_t Addressing::Indirect() const
{
    uint16_t addressPointer = mIcpu->fetchWord();
    return mIcpu->memory().readWord(addressPointer);
}
uint16_t Addressing::IndirectX() const
{
    uint16_t addressPointer = mIcpu->fetchWord() + mIcpu->registers().X;
    return mIcpu->memory().readWord(addressPointer);
}

uint16_t Addressing::IndirectY() const
{
    uint16_t addressPointer = mIcpu->fetchWord() + mIcpu->registers().Y;
    return mIcpu->memory().readWord(addressPointer);
}

std::function<uint16_t()> Addressing::createAddresingFunction(AddressingMode mode)
{
    switch (mode)
    {
        case AddressingMode::Implied: return std::bind(&Addressing::Implied, this);
        case AddressingMode::Immediate: return std::bind(&Addressing::Immediate, this);
        case AddressingMode::ZeroPage: return std::bind(&Addressing::ZeroPage, this);
        case AddressingMode::ZeroPageX: return std::bind(&Addressing::ZeroPageX, this);
        case AddressingMode::ZeroPageY: return std::bind(&Addressing::ZeroPageY, this);
        case AddressingMode::Relative: return std::bind(&Addressing::Relative, this);
        case AddressingMode::Absolute: return std::bind(&Addressing::Absolute, this);
        case AddressingMode::AbsoluteOffsetX: return std::bind(&Addressing::AbsoluteOffsetX, this);
        case AddressingMode::AbsoluteOffsetY: return std::bind(&Addressing::AbsoluteOffsetY, this);
        case AddressingMode::Indirect: return std::bind(&Addressing::Indirect, this);
        case AddressingMode::IndirectX: return std::bind(&Addressing::IndirectX, this);
        case AddressingMode::IndirectY: return std::bind(&Addressing::IndirectY, this);
    }
    return []() -> uint16_t {return 0;};
}
