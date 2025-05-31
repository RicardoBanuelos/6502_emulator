#include "PPU.h"
#include <iostream>
PPU::PPU()
{
}

PPU::~PPU()
{
}

void PPU::writeRegister(uint16_t addr, uint8_t data)
{
    mRegisters.write(addr, data);
}

uint8_t PPU::readRegister(uint16_t addr) const
{
    return mRegisters.read(addr);
}
