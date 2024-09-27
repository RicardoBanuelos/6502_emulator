#pragma once

#include <unordered_map>

#include "CPU/ICPU.h"
#include "Addressing.h"
#include "OpCodes/OpCodes.h"
#include "Instruction/Instruction.h"

class CPU : public ICPU, public std::enable_shared_from_this<CPU>
{
public:
    CPU();
    ~CPU();

    void init();
    void reset();
    void execute();

private:
    void initInstructions();

    uint8_t fetchByte() override;
    uint16_t fetchWord() override;
    void writeByte(uint8_t address, uint8_t byte) override;
    void writeWord(uint8_t address, uint16_t word) override;
    Registers &registers() override;
    Memory &memory() override;

    Registers mRegisters;
    Memory mMemory;
    std::unique_ptr<Addressing> mAddressingModes;
    std::unordered_map<OpCode, std::unique_ptr<Instruction>> mInstructions;
};