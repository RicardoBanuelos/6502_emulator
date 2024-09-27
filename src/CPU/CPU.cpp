#include "CPU/CPU.h"
#include "Instruction/Instructions/LDA.h"

CPU::CPU()
{
}

CPU::~CPU()
{
    
}

void CPU::init()
{
    mAddressingModes.reset(new Addressing(shared_from_this()));
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

Registers &CPU::registers()
{
    return mRegisters;
}
Memory &CPU::memory()
{
    return mMemory;
}
void CPU::execute()
{
    LDA  lda("hola", shared_from_this(), mAddressingModes->createAddresingFunction(AddressingMode::IndirectX), 100);
    lda.run();
}