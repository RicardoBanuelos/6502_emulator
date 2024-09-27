#pragma once

#include "Registers.h"
#include "CPU/ICPU.h"

class CPU : public ICPU
{
public:
    CPU();
    ~CPU();

    void reset();

private:
    uint8_t fetchByte();
    uint16_t fetchWord();
    void writeByte(uint8_t address, uint8_t byte);
    void writeWord(uint8_t address, uint16_t word);
    Registers &registers();
    Memory &memory();
    void execute();

    Registers mRegisters;
    Memory mMemory;
};