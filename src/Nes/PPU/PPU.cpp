#include "PPU.h"
#include <iostream>
PPU::PPU()
{

}

PPU::~PPU()
{
}

void PPU::writeRegister(uint32_t addr, uint8_t data)
{
    if(!isValidAddr(addr))
    {
        throw std::out_of_range("Address out of PPU register range");
    }

    uint8_t regIndex = addr & 0x7; // Get the register index (0-7)
    mRegisters.write(static_cast<PPURegisters::RegistersIndex>(regIndex), data);
}

uint8_t PPU::readRegister(uint32_t addr) const
{
    if(!isValidAddr(addr))
    {
        throw std::out_of_range("Address out of PPU register range");
    }

    uint8_t regIndex = addr & 0x7; // Get the register index (0-7)
    return mRegisters.read(static_cast<PPURegisters::RegistersIndex>(regIndex));
}

bool PPU::isValidAddr(uint32_t addr) const
{
    return (addr >= PPU_REGISTERS_START_ADDRESS && addr <= PPU_REGISTERS_END_ADDRESS);
}
