#include "CPU/Addressing.h"



Addressing::Addressing(std::shared_ptr<ICPU> icpu)
    :   mIcpu(icpu)
{
}

Addressing::~Addressing()
{
}

uint16_t Addressing::Implied() const
{
    return mIcpu->registers().A;
}

uint16_t Addressing::Accumulator() const
{
    return 0;
}
uint16_t Addressing::Immediate() const
{
    return mIcpu->fetchByte() & 0x00F;
}

uint16_t Addressing::ZeroPage() const
{
    uint16_t address = mIcpu->fetchByte() & 0x00FF;
    return address;
}

uint16_t Addressing::ZeroPageX() const
{
    uint16_t address = (mIcpu->fetchByte() + mIcpu->registers().X) & 0x00FF;
    return address;
}

uint16_t Addressing::ZeroPageY() const
{
    uint16_t address = (mIcpu->fetchByte() + mIcpu->registers().Y) & 0x00FF;
    return address;
}

uint16_t Addressing::Relative() const
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
    uint16_t address = mIcpu->fetchWord();
    return address;
}

uint16_t Addressing::AbsoluteOffsetX() const
{
    uint16_t address = mIcpu->fetchWord() + mIcpu->registers().X;
    return address;
}

uint16_t Addressing::AbsoluteOffsetY() const
{
    uint16_t address = mIcpu->fetchWord() + mIcpu->registers().Y;
    return address;
}


uint16_t Addressing::Indirect() const
{
    uint16_t addressPointer = mIcpu->fetchWord();
    return addressPointer;
}
uint16_t Addressing::IndirectX() const
{
    uint16_t addressPointer = mIcpu->fetchWord() + mIcpu->registers().X;
    return addressPointer;
}

uint16_t Addressing::IndirectY() const
{
    uint16_t addressPointer = mIcpu->fetchWord() + mIcpu->registers().Y;
    return addressPointer;
}

const std::function<uint16_t()> &Addressing::addressingFunction(AddressingMode mode)
{

    static std::function<uint16_t()> impledFunc = std::bind(&Addressing::Implied, this);
    static std::function<uint16_t()> AccumulatorFunc = std::bind(&Addressing::Accumulator, this);
    static std::function<uint16_t()> ImmediateFunc = std::bind(&Addressing::Immediate, this);
    static std::function<uint16_t()> ZeroPageFunc = std::bind(&Addressing::ZeroPage, this);
    static std::function<uint16_t()> ZeroPageXFunc = std::bind(&Addressing::ZeroPageX, this);
    static std::function<uint16_t()> ZeroPageYFunc = std::bind(&Addressing::ZeroPageY, this);
    static std::function<uint16_t()> RelativeFunc = std::bind(&Addressing::Relative, this);
    static std::function<uint16_t()> AbsoluteFunc = std::bind(&Addressing::Absolute, this);
    static std::function<uint16_t()> AbsoluteOffsetXFunc = std::bind(&Addressing::AbsoluteOffsetX, this);
    static std::function<uint16_t()> AbsoluteOffsetYFunc = std::bind(&Addressing::AbsoluteOffsetY, this);
    static std::function<uint16_t()> IndirectFunc = std::bind(&Addressing::Indirect, this);
    static std::function<uint16_t()> IndirectXFunc = std::bind(&Addressing::IndirectX, this);
    static std::function<uint16_t()> IndirectYFunc = std::bind(&Addressing::IndirectY, this);
    
    switch (mode)
    {
        case AddressingMode::Implied: return impledFunc;
        case AddressingMode::Immediate: return ImmediateFunc;
        case AddressingMode::ZeroPage: return ZeroPageFunc;
        case AddressingMode::ZeroPageX: return ZeroPageXFunc;
        case AddressingMode::ZeroPageY: return ZeroPageYFunc;
        case AddressingMode::Relative: return RelativeFunc;
        case AddressingMode::Absolute: return AbsoluteFunc;
        case AddressingMode::AbsoluteOffsetX: return AbsoluteOffsetXFunc;
        case AddressingMode::AbsoluteOffsetY: return AbsoluteOffsetYFunc;
        case AddressingMode::Indirect: return IndirectFunc;
        case AddressingMode::IndirectX: return IndirectXFunc;
        case AddressingMode::IndirectY: return IndirectYFunc;
    }

    return impledFunc;
}
