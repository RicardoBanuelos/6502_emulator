#include "CPU/CPU.h"
#include "Instruction/Instructions/LDA.h"
#include "Instruction/Instructions.h"

CPU::CPU()
{
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
    mRegisters.statusRegister.statusFlags.D = 0;
    mRegisters.A = 0;
    mRegisters.X = 0;
    mRegisters.Y = 0;
    mRegisters.statusRegister.byte = 0;
}

uint8_t CPU::fetchByte()
{
    uint8_t byte = mBus->readByte(mRegisters.PC);

    ++mRegisters.PC;

    return byte;
}

uint16_t CPU::fetchWord()
{
    uint16_t word = mBus->readWord(mRegisters.PC);

    ++mRegisters.PC;
    ++mRegisters.PC;

    return word;
}

void CPU::writeByte(uint16_t address, uint8_t byte)
{
    mBus->writeByte(address, byte);
}

void CPU::writeWord(uint16_t address, uint16_t word)
{
    mBus->writeWord(address, word);
}

void CPU::pushByte(uint8_t data)
{
    mBus->writeByte(mStackPointerOffset + mRegisters.SP, data);
    mRegisters.SP--;
}

uint8_t CPU::popByte()
{
    mRegisters.SP++;
    return mBus->readByte(mStackPointerOffset + mRegisters.SP);
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

uint8_t CPU::readByte(uint16_t address)
{
    return mBus->readByte(address);
}

uint16_t CPU::readWord(uint16_t address)
{
    return mBus->readWord(address);
}

void CPU::setFlag(Flag flag, bool value)
{
    mRegisters.statusRegister.setFlag(flag, value);
}

bool CPU::getFlag(Flag flag)
{
    return mRegisters.statusRegister.getFlag(flag);
}

void CPU::setRegister(Register reg, uint16_t value)
{
    switch (reg)
    {
        case PC:
            mRegisters.PC = value;
            break;

        case PS:
            mRegisters.statusRegister.byte = value;
            break;

        case SP:
            mRegisters.SP = value;
            break;

        case A:
            mRegisters.A = value;
            break;

        case X:
            mRegisters.X = value;
            break;

        case Y:
            mRegisters.Y = value;
            break;
        
        default:
            break;
    }
}

uint16_t CPU::getRegister(Register reg)
{
    switch (reg)
    {
        case PC:
            return mRegisters.PC;

        case PS:
            return mRegisters.statusRegister.byte;

        case SP:
            return mRegisters.SP;

        case A:
            return mRegisters.A;

        case X:
            return mRegisters.X;

        case Y:
            return mRegisters.Y;
        
        default:
            return -1;
    }
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

void CPU::connectBus(std::shared_ptr<IBus> bus)
{
    mBus = bus;
}
AddressingData CPU::addressing(AddressingMode mode) const
{
    return mAddressingModes->addressing(mode);
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
    // BNE
    mInstructions[OC_BNE_RELATIVE].reset(new BNE(shared_from_this(), AddressingMode::Relative, 2));
    // BPL
    mInstructions[OC_BPL_RELATIVE].reset(new BPL(shared_from_this(), AddressingMode::Relative, 2));
}