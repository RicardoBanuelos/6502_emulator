#include "CPU/Addressing.h"


Addressing::Addressing(std::shared_ptr<ICPU> icpu)
    :   mIcpu(icpu)
{
}

Addressing::~Addressing()
{
}

AddressingData Addressing::Implied() const
{
    return AddressingData(0, mIcpu->registers().A);
}

AddressingData Addressing::Accumulator() const
{
    return AddressingData(0, 0);
}
AddressingData Addressing::Immediate() const
{
    return AddressingData(0, mIcpu->fetchByte() & 0x00F);
}

AddressingData Addressing::ZeroPage() const
{
    uint16_t address = mIcpu->fetchByte() & 0x00FF;
    uint8_t data = mIcpu->memory().readByte(address);

    return AddressingData(address, data);
}

AddressingData Addressing::ZeroPageX() const
{
    uint16_t address = (mIcpu->fetchByte() + mIcpu->registers().X) & 0x00FF;
    uint8_t data = mIcpu->memory().readByte(address);

    return AddressingData(address, data);
}

AddressingData Addressing::ZeroPageY() const
{
    uint16_t address = (mIcpu->fetchByte() + mIcpu->registers().Y) & 0x00FF;
    uint8_t data = mIcpu->memory().readByte(address);

    return AddressingData(address, data);
}

AddressingData Addressing::Relative() const
{
    uint8_t relativeAddress = mIcpu->fetchByte();
    if(relativeAddress & 0x80)
    {
        relativeAddress |= 0xFF00;
    }

    return AddressingData(relativeAddress, 0);
}

AddressingData Addressing::Absolute() const
{
    uint16_t address = mIcpu->fetchWord();
    uint8_t data = mIcpu->memory().readByte(address);

    return AddressingData(address, data);
}

AddressingData Addressing::AbsoluteOffsetX() const
{
    uint16_t address = mIcpu->fetchWord() + mIcpu->registers().X;
    uint8_t data = mIcpu->memory().readByte(address);
    return AddressingData(address, data);
}

AddressingData Addressing::AbsoluteOffsetY() const
{
    uint16_t address = mIcpu->fetchWord() + mIcpu->registers().Y;
    uint8_t data = mIcpu->memory().readByte(address);
    return AddressingData(address, data);
}


AddressingData Addressing::Indirect() const
{
    uint16_t addressPointer = mIcpu->fetchWord();
    uint16_t data = mIcpu->memory().readWord(addressPointer);

    return AddressingData(addressPointer, data);
}
AddressingData Addressing::IndirectX() const
{
    uint16_t addressPointer = mIcpu->fetchWord() + mIcpu->registers().X;
    uint16_t data = mIcpu->memory().readWord(addressPointer);

    return AddressingData(addressPointer, data);
}

AddressingData Addressing::IndirectY() const
{
    uint16_t addressPointer = mIcpu->fetchWord() + mIcpu->registers().Y;
    uint16_t data = mIcpu->memory().readWord(addressPointer);

    return AddressingData(addressPointer, data);
}

const std::function<AddressingData()> &Addressing::addressingFunction(AddressingMode mode)
{

    static std::function<AddressingData()> impledFunc = std::bind(&Addressing::Implied, this);
    static std::function<AddressingData()> ImmediateFunc = std::bind(&Addressing::Immediate, this);
    static std::function<AddressingData()> ZeroPageFunc = std::bind(&Addressing::ZeroPage, this);
    static std::function<AddressingData()> ZeroPageXFunc = std::bind(&Addressing::ZeroPageX, this);
    static std::function<AddressingData()> ZeroPageYFunc = std::bind(&Addressing::ZeroPageY, this);
    static std::function<AddressingData()> RelativeFunc = std::bind(&Addressing::Relative, this);
    static std::function<AddressingData()> AbsoluteFunc = std::bind(&Addressing::Absolute, this);
    static std::function<AddressingData()> AbsoluteOffsetXFunc = std::bind(&Addressing::AbsoluteOffsetX, this);
    static std::function<AddressingData()> AbsoluteOffsetYFunc = std::bind(&Addressing::AbsoluteOffsetY, this);
    static std::function<AddressingData()> IndirectFunc = std::bind(&Addressing::Indirect, this);
    static std::function<AddressingData()> IndirectXFunc = std::bind(&Addressing::IndirectX, this);
    static std::function<AddressingData()> IndirectYFunc = std::bind(&Addressing::IndirectY, this);
    static std::function<AddressingData()> AccumulatorFunc = std::bind(&Addressing::Accumulator, this);
    
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
        case AddressingMode::Accumulator: return AccumulatorFunc;
        
    }

    return impledFunc;
}