#include "CPU/CPU.h"
#include "Instruction/Instructions/LDA.h"
#include "Instruction/Instructions.h"

CPU::CPU()
{
    mMemory.initialize();
}

CPU::~CPU()
{
    
}

void CPU::init()
{
    mAddressingModes.reset(new Addressing(shared_from_this()));
    initInstructions();
}

void CPU::reset()
{
    mRegisters.PC = 0xFFCC;
    mRegisters.SP = 0xFF;
    mRegisters.status.D = 0;
    mRegisters.A = 0;
    mRegisters.X = 0;
    mRegisters.Y = 0;
    mRegisters.status.byte = 0;
}

uint8_t CPU::fetchByte()
{
    uint8_t byte = mMemory.readByte(mRegisters.PC);

    ++mRegisters.PC;

    return byte;
}

uint16_t CPU::fetchWord()
{
    uint8_t word = mMemory.readWord(mRegisters.PC);

    ++mRegisters.PC;
    ++mRegisters.PC;

    return word;
}

void CPU::writeByte(uint8_t address, uint8_t byte)
{
    mMemory.writeByte(address, byte);
}

void CPU::writeWord(uint8_t address, uint16_t word)
{
    mMemory.writeWord(address, word);
}

void CPU::pushByte(uint8_t data)
{
    mMemory.writeByte(mStackPointerOffset + mRegisters.SP, data);
    mRegisters.SP--;
}

uint8_t CPU::popByte()
{
    mRegisters.SP--;
    return mMemory.readByte(mStackPointerOffset + mRegisters.SP);
}

void CPU::pushWord(uint16_t data)
{
    pushByte((data & 0xFF00) >> 8);
    pushByte(data & 0x00FF);
}

uint16_t CPU::popWord()
{
    uint16_t data = 0;
    uint8_t lo = popByte();
    uint8_t hi = popByte();

    return lo | (hi << 8 & 0xFF00);
}


Registers& CPU::registers()
{
    return mRegisters;
}
Memory& CPU::memory()
{
    return mMemory;
}
void CPU::execute()
{
    OpCode opCode = static_cast<OpCode>(fetchByte());

    if(mInstructions.count(opCode) == 0)
    {
        return;
    }

    mInstructions.at(opCode)->run();
}

const AddressingData CPU::addressing(AddressingMode mode) const
{
    return mAddressingModes->addressingFunction(mode)();
}
void CPU::initInstructions()
{
    // ADC
    mInstructions[OC_ADC_IMMEDIATE].reset(new ADC(shared_from_this(), AddressingMode::Immediate, 2));
    mInstructions[OC_ADC_ZERO_PAGE].reset(new ADC(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_ADC_ZERO_PAGE_X].reset(new ADC(shared_from_this(), AddressingMode::ZeroPageX, 4));
    mInstructions[OC_ADC_ABSOLUTE].reset(new ADC(shared_from_this(), AddressingMode::Absolute, 4));
    mInstructions[OC_ADC_ABSOLUTE_X].reset(new ADC(shared_from_this(), AddressingMode::AbsoluteOffsetX, 4));
    mInstructions[OC_ADC_ABSOLUTE_Y].reset(new ADC(shared_from_this(), AddressingMode::AbsoluteOffsetY, 4));
    mInstructions[OC_ADC_INDIRECT_X].reset(new ADC(shared_from_this(), AddressingMode::IndirectX, 6));
    mInstructions[OC_ADC_INDIRECT_Y].reset(new ADC(shared_from_this(), AddressingMode::IndirectY, 5));
    // AND
    mInstructions[OC_AND_IMMEDIATE].reset(new AND(shared_from_this(), AddressingMode::Immediate, 2));
    mInstructions[OC_AND_ZERO_PAGE].reset(new AND(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_AND_ZERO_PAGE_X].reset(new AND(shared_from_this(), AddressingMode::ZeroPageX, 4));
    mInstructions[OC_AND_ABSOLUTE].reset(new AND(shared_from_this(), AddressingMode::Absolute, 4));
    mInstructions[OC_AND_ABSOLUTE_X].reset(new AND(shared_from_this(), AddressingMode::AbsoluteOffsetX, 4));
    mInstructions[OC_AND_ABSOLUTE_Y].reset(new AND(shared_from_this(), AddressingMode::AbsoluteOffsetY, 4));
    mInstructions[OC_AND_INDIRECT_X].reset(new AND(shared_from_this(), AddressingMode::IndirectX, 6));
    mInstructions[OC_AND_INDIRECT_Y].reset(new AND(shared_from_this(), AddressingMode::IndirectY, 5));
    // ASL
    mInstructions[OC_ASL_ACCUMULATOR].reset(new ASL(shared_from_this(), AddressingMode::Implied, 2));
    mInstructions[OC_ASL_ZERO_PAGE].reset(new ASL(shared_from_this(), AddressingMode::ZeroPage, 5));
    mInstructions[OC_ASL_ZERO_PAGE_X].reset(new ASL(shared_from_this(), AddressingMode::ZeroPageX, 6));
    mInstructions[OC_ASL_ABSOLUTE].reset(new ASL(shared_from_this(), AddressingMode::Absolute, 6));
    mInstructions[OC_ASL_ABSOLUTE_X].reset(new ASL(shared_from_this(), AddressingMode::AbsoluteOffsetX, 7));
    // BCC
    mInstructions[OC_BCC_RELATIVE].reset(new BCC(shared_from_this(), AddressingMode::Relative, 2));
    // BCS
    mInstructions[OC_BCS_RELATIVE].reset(new BCS(shared_from_this(), AddressingMode::Relative, 2));
    // BIT
    mInstructions[OC_BIT_ZERO_PAGE].reset(new BIT(shared_from_this(), AddressingMode::ZeroPage, 3));
    mInstructions[OC_BIT_ABSOLUTE].reset(new BIT(shared_from_this(), AddressingMode::Absolute, 4));
    // BMI
    mInstructions[OC_BMI_RELATIVE].reset(new BMI(shared_from_this(), AddressingMode::Relative, 2));
}