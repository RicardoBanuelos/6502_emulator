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
    return 0;
}

uint16_t CPU::fetchWord()
{
    return 0;
}

void CPU::execute()
{

}