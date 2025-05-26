#pragma once

#include "Registers.h"
#include "IPPU.h"

class PPU : public IPPU
{

public:
    PPU();
    ~PPU();


    void writeRegister(uint32_t addr, uint8_t data) override;
    uint8_t readRegister(uint32_t addr) const override;

private:
    PPURegisters mRegisters;
    static const uint16_t PPU_REGISTERS_START_ADDRESS = 0x2000;
    static const uint16_t PPU_REGISTERS_END_ADDRESS = 0x2007; // Last PPU register address

    bool isValidAddr(uint32_t addr) const;
};
