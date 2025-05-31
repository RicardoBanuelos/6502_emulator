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

uint8_t NesBus::readByte(uint32_t address)
{
    address = mapAddress(address);

    if (address <= RAM_LAST_MIRRORED_ADDRESS)
    {
        // Internal RAM
        return mMemory->readByte(address);
    }
    else if (address <= PPU_LAST_MIRRORED_ADDRESS)
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
    return 0; // Placeholder for actual PRG-ROM read
}

void NesBus::writeByte(uint32_t address, uint8_t data)
{
    address = mapAddress(address);

    if (address <= RAM_LAST_MIRRORED_ADDRESS)
    {
        // Internal RAM
        mMemory->writeByte(address, data); // Placeholder for actual RAM write
    }
    else if (address <= PPU_LAST_MIRRORED_ADDRESS)
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
    }
}


uint16_t NesBus::readWord(uint32_t address)
{
    // Handle edge cases when reading across memory region boundaries
    // For example, reading a word at 0x07FF would cross from RAM into PPU registers
    if (address == RAM_LAST_MIRRORED_ADDRESS) {
        uint8_t low = readByte(address);
        address = mapAddress(address + 1); // Move to next address for high byte
        uint8_t high = readByte(address); // This will be in PPU register space
        return (static_cast<uint16_t>(high) << 8) | low;
        // uint8_t high = readByte(address + 1); // This will be in PPU register space
        // return (static_cast<uint16_t>(high) << 8) | low;
    }

    address = mapAddress(address);

    if (address <= RAM_LAST_MIRRORED_ADDRESS)
    {
        // Internal RAM
        return mMemory->readWord(address); // Placeholder for actual RAM read
    }
    else if (address <= PPU_LAST_MIRRORED_ADDRESS)
    {
        // PPU Registers
        // Note: PPU registers are typically accessed as bytes, but if a word read is needed,
        throw std::runtime_error("Word read from PPU registers is not supported.");
    }
    else if (address <= 0x401F)
    {
        // APU and I/O registers
        // Note: APU/I/O registers are typically accessed as bytes, but if a word read is needed,
        throw std::runtime_error("Word read from APU/I/O registers is not supported.");
    }

    // Cartridge PRG-ROM
    // Note: Reading from PRG-ROM is typically done as bytes, but if a word read is needed,
    throw std::runtime_error("Word read from PRG-ROM is not supported.");
}

void NesBus::writeWord(uint32_t address, uint16_t data)
{

    // Handle edge cases when writing across memory region boundaries
    // For example, writing a word at 0x07FF would cross from RAM into PPU registers
    if (address == RAM_LAST_MIRRORED_ADDRESS) {
        writeByte(address, static_cast<uint8_t>(data & 0xFF)); // Low byte
        address = mapAddress(address + 1); // Move to next address for high byte
        writeByte(address, static_cast<uint8_t>((data >> 8) & 0xFF)); // High byte
        return;
    }

    address = mapAddress(address);

    if (address <= RAM_LAST_MIRRORED_ADDRESS)
    {
        // Internal RAM
        mMemory->writeWord(address, data); // Placeholder for actual RAM write
    }
    else if (address <= PPU_LAST_MIRRORED_ADDRESS)
    {
        // PPU Registers
        // Note: PPU registers are typically accessed as bytes, but if a word write is needed,
        throw std::runtime_error("Word write to PPU registers is not supported.");
    }
    else if (address <= 0x401F)
    {
        // APU and I/O registers
        // Note: APU/I/O registers are typically accessed as bytes, but if a word write is needed,
        throw std::runtime_error("Word write to APU/I/O registers is not supported.");
    }
    else
    {
        // Cartridge PRG-ROM
        // Note: Writing to PRG-ROM is not typical, but if needed, it should be handled by the mapper
        throw std::runtime_error("Writing to PRG-ROM is not supported.");
        // Placeholder for actual PRG-ROM write
    }
}


uint32_t NesBus::mapAddress(uint32_t address) const
{
    if (address <= RAM_LAST_ADDRESS)
    {
        // Internal RAM (2KB), mirrored every 2KB up to $1FFF
        address = address & 0x07FF;
    }
    else if (address <= PPU_LAST_ADDRESS)
    {
        // PPU Registers ($2000-$2007), mirrored every 8 bytes up to $3FFF
        address = 0x2000 + (address & 0x0007);
    }
    // $4000-$401F: APU and I/O registers (no mirroring here)
    // $4020-$FFFF: Cartridge space (no mirroring here)
    // These ranges are passed through as-is

    return address;
}

void NesBus::connectPPU(std::shared_ptr<IPPU> ppu)
{
    mPPU = ppu;
}
