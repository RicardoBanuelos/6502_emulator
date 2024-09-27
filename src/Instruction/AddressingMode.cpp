#include "Instruction/Addressing.h"


Addressing::Addressing(std::shared_ptr<ICPU> icpu)
    :   mIcpu(icpu)
{
}

Addressing::~Addressing()
{
}

uint8_t Addressing::Implied() const
{
    return 0;
}

uint8_t Addressing::Immediate() const
{
    return mIcpu->fetchByte();
}

uint8_t Addressing::ZeroPage() const
{
    return mIcpu->fetchByte() & 0x00FF;
}

uint8_t Addressing::ZeroPageX() const
{
    return mIcpu->fetchByte() + mIcpu->registers().X;
}

uint8_t Addressing::ZeroPageY() const
{
    return mIcpu->fetchByte() + mIcpu->registers().Y;
}

uint8_t Addressing::Relative() const
{
    uint8_t relativeAddress = mIcpu->fetchByte();
    if(relativeAddress & 0x80)
    {
        relativeAddress |= 0xFF00;
    }

    return relativeAddress;
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
    return Indirect() + mIcpu->registers().X;
}

uint16_t Addressing::IndirectY() const
{
    return Indirect() + mIcpu->registers().Y;
}
