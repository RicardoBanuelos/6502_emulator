#include "NesBus.h"
#include <iostream>
/*
    $0000–$07FF	2 KB	    Internal RAM (mirrored every 2 KB)
    $0800–$1FFF	6 KB	    Mirrors of $0000–$07FF
    $2000–$2007	8 bytes	    PPU Registers
    $2008–$3FFF	8 KB	    Mirrors of $2000–$2007
    $4000–$4017	24 bytes	APU & I/O Registers
    $4018–$401F	8 bytes	    APU & I/O Test Mode
    $4020–$5FFF	8 KB	    Cartridge Expansion ROM (rare)
    $6000–$7FFF	8 KB	    Cartridge SRAM (battery-backed RAM)
    $8000–$FFFF	32 KB	    Cartridge PRG-ROM (game code/data)
*/

NesBus::NesBus()
{
}

NesBus::~NesBus()
{
}

uint8_t NesBus::readByte(uint16_t address)
{
    address = mapAddress(address);

    if (address <= RAM_LAST_MIRRORED_ADDRESS)
    {
        // Internal RAM
        return mMemory->readByte(address);
    }
    else if (address <= PPU_LAST_ADDRESS)
    {
        // PPU Registers
        return mPPU->readRegister(address); // Placeholder for actual PPU register read
    }
    else if (address <= APU_IO_LAST_ADDRESS)
    {
        // APU and I/O registers
        return 0; // Placeholder for actual APU/I/O register read
    }

    // Cartridge PRG-ROM
    return mCartridge->readByte(address); // Placeholder for actual PRG-ROM read
}

void NesBus::writeByte(uint16_t address, uint8_t data)
{
    address = mapAddress(address);

    if (address <= RAM_LAST_MIRRORED_ADDRESS)
    {
        // Internal RAM
        mMemory->writeByte(address, data); // Placeholder for actual RAM write
    }
    else if (address <= PPU_LAST_ADDRESS)
    {
        // PPU Registers
        mPPU->writeRegister(address, data);
        // Placeholder for actual PPU register write
    }
    else if (address <= APU_IO_LAST_ADDRESS)
    {
        // APU and I/O registers
        // Placeholder for actual APU/I/O register write

    }
    else
    {
        // Cartridge PRG-ROM
        // Placeholder for actual PRG-ROM write
        mCartridge->writeByte(address, data);
    }
}


uint16_t NesBus::readWord(uint16_t address) {
    // Standard 6502 behavior - if reading a word at 0xFFFF, it wraps around to 0x0000
    uint8_t low = readByte(address);
    uint8_t high = readByte((address + 1) & 0xFFFF); // Apply 16-bit wrap
    
    return (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
}

void NesBus::writeWord(uint16_t address, uint16_t data)
{
    uint16_t low = data & 0x00FF;
    uint16_t high = (data >> 8) & 0x00FF;
    writeByte(address, static_cast<uint8_t>(low));
    writeByte(address + 1, static_cast<uint8_t>(high));
}


uint16_t NesBus::mapAddress(uint16_t address) const
{
    if (address <= RAM_LAST_ADDRESS)
    {
        // Internal RAM (2KB), mirrored every 2KB up to $1FFF
        address = address & RAM_LAST_MIRRORED_ADDRESS;
    }
    // $2000-$3FFF: PPU registers (mirrored every 8 bytes)
    // $4000-$401F: APU and I/O registers (no mirroring here)
    // $4020-$FFFF: Cartridge space (no mirroring here)
    // These ranges are passed through as-is

    return address;
}

void NesBus::connectPPU(std::shared_ptr<IPPU> ppu)
{
    mPPU = ppu;
}

void NesBus::connectCatridge(std::shared_ptr<ICatridge> cartridge)
{
    mCartridge = cartridge;
}