#include "Addressing.h"


Addressing::Addressing(std::shared_ptr<ICPU> icpu)
    :   mIcpu(icpu)
{
}

Addressing::~Addressing()
{
    
}

AddressingData Addressing::Implied() const
{
    return AddressingData(0, mIcpu->getRegister(Register::A));
}

AddressingData Addressing::Accumulator() const
{
    return AddressingData(0, mIcpu->getRegister(Register::A));
}

AddressingData Addressing::Immediate() const
{
    return AddressingData(0, mIcpu->fetchByte());
}

AddressingData Addressing::ZeroPage() const
{
    uint16_t address = mIcpu->fetchByte();
    uint8_t data = mIcpu->readByte(address);

    return AddressingData(address, data);
}

AddressingData Addressing::ZeroPageX() const
{
    uint16_t address = (mIcpu->fetchByte() + mIcpu->getRegister(Register::X)) & 0xFF;
    uint8_t data = mIcpu->readByte(address);

    return AddressingData(address, data);
}

AddressingData Addressing::ZeroPageY() const
{
    uint16_t address = (mIcpu->fetchByte() + mIcpu->getRegister(Register::Y)) & 0xFF;
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
    uint16_t address = mIcpu->fetchWord() + mIcpu->getRegister(Register::X);
    uint8_t data = mIcpu->readByte(address);
    return AddressingData(address, data);
}

AddressingData Addressing::AbsoluteOffsetY() const
{
    uint16_t address = mIcpu->fetchWord() + mIcpu->getRegister(Register::Y);
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
    // Apply zero page wrap when adding X register to zero page address
    uint16_t zeroPageAddr = (mIcpu->fetchByte() + mIcpu->getRegister(Register::X)) & 0xFF;
    
    // Read the target address from the wrapped zero page location
    uint16_t indirect = mIcpu->readWord(zeroPageAddr);
    uint8_t data = mIcpu->readByte(indirect);

    return AddressingData(indirect, data);
}

AddressingData Addressing::IndirectY() const
{
    uint16_t zeroPageAddress = mIcpu->fetchByte();
    uint16_t indirect = mIcpu->readWord(zeroPageAddress);
    uint16_t finalAddress = indirect + mIcpu->getRegister(Register::Y);
    uint8_t data = mIcpu->readByte(finalAddress);

    return AddressingData(finalAddress, data);
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
        case AddressingMode::AbsoluteX: return AbsoluteOffsetX();
        case AddressingMode::AbsoluteY: return AbsoluteOffsetY();
        case AddressingMode::Indirect: return Indirect();
        case AddressingMode::IndirectX: return IndirectX();
        case AddressingMode::IndirectY: return IndirectY();
        case AddressingMode::Accumulator: return Accumulator();
        
    }

    return AddressingData(0,0);
}