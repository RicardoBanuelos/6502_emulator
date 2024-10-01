#pragma once

#include <unordered_map>

#include "CPU/ICPU.h"
#include "OpCodes/OpCodes.h"
#include "Instruction/Instruction.h"
#include "Bus/IBus.h"

class CPU : public ICPU, public std::enable_shared_from_this<CPU>
{
public:
    CPU();
    ~CPU();

    void init();
    void reset();
    void execute();
    void connectBus(std::shared_ptr<IBus> bus);

    AddressingData addressing(AddressingMode mode) const override;
private:
    void initInstructions();

    uint8_t fetchByte() override;
    uint16_t fetchWord() override;
    
    void writeByte(uint16_t address, uint8_t byte) override;
    void writeWord(uint16_t address, uint16_t word) override;

    void pushByte(uint8_t data);
    uint8_t popByte();
    void pushWord(uint16_t data);
    uint16_t popWord();

    uint8_t readByte(uint16_t address) override;
    uint16_t readWord(uint16_t address) override;

    void setFlag(Flag flag, bool value) override;
    bool getFlag(Flag flag) override;
    void setRegister(Register register, uint16_t value) override;
    uint16_t getRegister(Register register) override;

    Registers mRegisters;
    std::unique_ptr<Addressing> mAddressingModes;
    std::unordered_map<OpCode, std::unique_ptr<Instruction>> mInstructions;
    std::shared_ptr<IBus> mBus;
    const uint16_t mStackPointerOffset = 0x0100;
};