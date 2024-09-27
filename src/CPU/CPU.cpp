#include "CPU/CPU.h"

CPU::CPU(){}

CPU::~CPU(){}

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

void CPU::execute()
{
    // TO DO
}