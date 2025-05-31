#include "Addressing.h"

Addressing::Addressing(std::shared_ptr<ICPU> icpu)
    : mIcpu(icpu)
{
}

Addressing::~Addressing()
{
    
}

AddressingData Addressing::Implied() const
{
    // Implied addressing doesn't access memory, so no page boundary crossing
    return AddressingData(0, mIcpu->getRegister(Register::A), 0);
}

AddressingData Addressing::Accumulator() const
{
    // Accumulator mode doesn't access memory, so no page boundary crossing
    return AddressingData(0, mIcpu->getRegister(Register::A), 0);
}

AddressingData Addressing::Immediate() const
{
    // Immediate addressing doesn't involve page boundary crossing
    return AddressingData(0, mIcpu->fetchByte(), 0);
}

AddressingData Addressing::ZeroPage() const
{
    // Zero Page addressing is always within page 0, so no page crossing
    uint16_t address = mIcpu->fetchByte();
    uint8_t data = mIcpu->readByte(address);
    return AddressingData(address, data, 0);
}

AddressingData Addressing::ZeroPageX() const
{
    // Zero Page X is always within page 0 (wraps within the page), so no page crossing
    uint16_t address = (mIcpu->fetchByte() + mIcpu->getRegister(Register::X)) & 0xFF;
    uint8_t data = mIcpu->readByte(address);
    return AddressingData(address, data, 0);
}

AddressingData Addressing::ZeroPageY() const
{
    // Zero Page Y is always within page 0 (wraps within the page), so no page crossing
    uint16_t address = (mIcpu->fetchByte() + mIcpu->getRegister(Register::Y)) & 0xFF;
    uint8_t data = mIcpu->readByte(address);
    return AddressingData(address, data, 0);
}

AddressingData Addressing::Relative() const
{
    // Relative addressing for branch instructions
    // Page boundary crossing is handled during branch execution, not in addressing mode
    uint8_t relativeAddress = mIcpu->fetchByte();
    int8_t offset = static_cast<int8_t>(relativeAddress); // Treat as signed for branch
    
    // We return the offset, and branch instructions will determine if page crossed
    return AddressingData(offset, 0, 0);
}

AddressingData Addressing::Absolute() const
{
    // Absolute addressing doesn't have variable cycles for page crossing
    uint16_t address = mIcpu->fetchWord();
    uint8_t data = mIcpu->readByte(address);
    return AddressingData(address, data, 0);
}

AddressingData Addressing::AbsoluteOffsetX() const
{
    // Calculate base address and final address
    uint16_t baseAddress = mIcpu->fetchWord();
    uint16_t finalAddress = baseAddress + mIcpu->getRegister(Register::X);
    uint8_t data = mIcpu->readByte(finalAddress);
    
    // Check for page boundary crossing
    int extraCycles = ((baseAddress & 0xFF00) != (finalAddress & 0xFF00)) ? 1 : 0;
    
    return AddressingData(finalAddress, data, extraCycles);
}

AddressingData Addressing::AbsoluteOffsetY() const
{
    // Calculate base address and final address
    uint16_t baseAddress = mIcpu->fetchWord();
    uint16_t finalAddress = baseAddress + mIcpu->getRegister(Register::Y);
    uint8_t data = mIcpu->readByte(finalAddress);
    
    // Check for page boundary crossing
    int extraCycles = ((baseAddress & 0xFF00) != (finalAddress & 0xFF00)) ? 1 : 0;
    
    return AddressingData(finalAddress, data, extraCycles);
}

AddressingData Addressing::Indirect() const
{
    // Indirect addressing doesn't have variable cycles for page crossing
    uint16_t addressPointer = mIcpu->fetchWord();
    uint16_t data = mIcpu->readWord(addressPointer);
    return AddressingData(addressPointer, data, 0);
}

AddressingData Addressing::IndirectX() const
{
    // IndirectX doesn't cross page boundaries in the address calculation
    // since it wraps in zero page
    uint16_t zeroPageAddr = (mIcpu->fetchByte() + mIcpu->getRegister(Register::X)) & 0xFF;
    uint16_t indirect = mIcpu->readWord(zeroPageAddr);
    uint8_t data = mIcpu->readByte(indirect);
    return AddressingData(indirect, data, 0);
}

AddressingData Addressing::IndirectY() const
{
    // Get zero page address and base indirect address
    uint16_t zeroPageAddress = mIcpu->fetchByte();
    uint16_t baseIndirect = mIcpu->readWord(zeroPageAddress);
    
    // Calculate final address with Y offset
    uint16_t finalAddress = baseIndirect + mIcpu->getRegister(Register::Y);
    uint8_t data = mIcpu->readByte(finalAddress);
    
    // Check for page boundary crossing
    int extraCycles = ((baseIndirect & 0xFF00) != (finalAddress & 0xFF00)) ? 1 : 0;
    
    return AddressingData(finalAddress, data, extraCycles);
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
    return AddressingData(0, 0, 0);
}