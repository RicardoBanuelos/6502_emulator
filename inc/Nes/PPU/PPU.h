#pragma once

#include "Registers.h"
#include "IPPU.h"

class PPU : public IPPU
{

public:
    PPU();
    ~PPU();


    void writeRegister(uint16_t addr, uint8_t data) override;
    uint8_t readRegister(uint16_t addr) const override;

private:
    PPURegisters mRegisters;
};
