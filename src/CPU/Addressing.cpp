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
    return AddressingData(0, mIcpu->registers().A);
}

AddressingData Addressing::Immediate() const
{
    return AddressingData(0, mIcpu->fetchByte() & 0x00FF);
}

AddressingData Addressing::ZeroPage() const
{
    uint16_t address = mIcpu->fetchByte() & 0x00FF;
    uint8_t data = mIcpu->readByte(address);

    return AddressingData(address, data);
}

AddressingData Addressing::ZeroPageX() const
{
    uint16_t address = (mIcpu->fetchByte() + mIcpu->registers().X) & 0x00FF;
    uint8_t data = mIcpu->readByte(address);

    return AddressingData(address, data);
}

AddressingData Addressing::ZeroPageY() const
{
    uint16_t address = (mIcpu->fetchByte() + mIcpu->registers().Y) & 0x00FF;
    uint8_t data = mIcpu->readByte(address);

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
    uint8_t data = mIcpu->readByte(address);

    return AddressingData(address, data);
}

AddressingData Addressing::AbsoluteOffsetX() const
{
    uint16_t address = mIcpu->fetchWord() + mIcpu->registers().X;
    uint8_t data = mIcpu->readByte(address);
    return AddressingData(address, data);
}

AddressingData Addressing::AbsoluteOffsetY() const
{
    uint16_t address = mIcpu->fetchWord() + mIcpu->registers().Y;
    uint8_t data = mIcpu->readByte(address);
    return AddressingData(address, data);
}


AddressingData Addressing::Indirect() const
{
    uint16_t addressPointer = mIcpu->fetchWord();
    uint16_t data = mIcpu->readWord(addressPointer);

    return AddressingData(addressPointer, data);
}
AddressingData Addressing::IndirectX() const
{
    uint16_t addressPointer = mIcpu->fetchWord() + mIcpu->registers().X;
    uint16_t data = mIcpu->readWord(addressPointer);

    return AddressingData(addressPointer, data);
}

AddressingData Addressing::IndirectY() const
{
    uint16_t addressPointer = mIcpu->fetchWord() + mIcpu->registers().Y;
    uint16_t data = mIcpu->readWord(addressPointer);

    return AddressingData(addressPointer, data);
}

AddressingData Addressing::addressing(AddressingMode mode)
{

    switch (mode)
    {
        case AddressingMode::Implied: return Implied();
        case AddressingMode::Immediate: return Immediate();
        case AddressingMode::ZeroPage: return ZeroPage();
        case AddressingMode::ZeroPageX: return ZeroPageX();
        case AddressingMode::ZeroPageY: return ZeroPageY();
        case AddressingMode::Relative: return Relative();
        case AddressingMode::Absolute: return Absolute();
        case AddressingMode::AbsoluteOffsetX: return AbsoluteOffsetX();
        case AddressingMode::AbsoluteOffsetY: return AbsoluteOffsetY();
        case AddressingMode::Indirect: return Indirect();
        case AddressingMode::IndirectX: return IndirectX();
        case AddressingMode::IndirectY: return IndirectY();
        case AddressingMode::Accumulator: return Accumulator();
        
    }

    return AddressingData(0,0);
}