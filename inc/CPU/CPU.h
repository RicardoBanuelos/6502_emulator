#pragma once

#include "CPU/ICPU.h"
#include "AddressingMode.h"

class CPU : public ICPU
{
public:
    CPU();
    virtual ~CPU();

    void reset();

private:
    uint8_t fetchByte() override;
    uint16_t fetchWord() override;
    void writeByte(uint8_t address, uint8_t byte) override;
    void writeWord(uint8_t address, uint16_t word) override;
    Registers &registers() override;
    Memory &memory() override;
    void execute();

    Registers mRegisters;
    Memory mMemory;
    Addressing mAddressingModes;
};