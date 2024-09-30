#include "CPU/CPU.h"
#include "Instruction/Instructions/LDA.h"


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

void CPU::writeByte(uint16_t address, uint8_t byte)
{
    mMemory.writeByte(address, byte);
}

void CPU::writeWord(uint16_t address, uint16_t word)
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
    // mInstructions[OC_ADC_IMMEDIATE] = std::make_unique<Instruction>(nullptr);
    // mInstructions[OC_ADC_ZERO_PAGE] = std::make_unique<Instruction>(nullptr);
    // mInstructions[OC_ADC_ZERO_PAGE_X] = std::make_unique<Instruction>(nullptr);
    // mInstructions[OC_ADC_ABSOLUTE] = std::make_unique<Instruction>(nullptr);
    // mInstructions[OC_ADC_ABSOLUTE_X] = std::make_unique<Instruction>(nullptr);
    // mInstructions[OC_ADC_ABSOLUTE_Y] = std::make_unique<Instruction>(nullptr);
    // mInstructions[OC_ADC_INDIRECT_X] = std::make_unique<Instruction>(nullptr);
    // mInstructions[OC_ADC_INDIRECT_Y] = std::make_unique<Instruction>(nullptr);
}