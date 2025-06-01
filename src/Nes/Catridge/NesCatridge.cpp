#include "NesCatridge.h"

NesCatridge::NesCatridge()
    : mExpansionRom(std::make_unique<Memory>(8 * 1024)), // 8 KB
      mSram(std::make_unique<Memory>(8 * 1024)),         // 8 KB
      mPrgRom(std::make_unique<Memory>(32 * 1024))       // 32 KB
{
}

NesCatridge::~NesCatridge()
{
}

uint8_t NesCatridge::readByte(uint16_t address) const
{
    if (address >= EXPANSION_ROM_START_ADDRESS && address <= EXPANSION_ROM_END_ADDRESS)
    {
        return mExpansionRom->readByte(address - EXPANSION_ROM_START_ADDRESS);
    }
    else if (address >= SRAM_ROM_START_ADDRESS && address <= SRAM_ROM_END_ADDRESS)
    {
        return mSram->readByte(address - SRAM_ROM_START_ADDRESS);
    }
    else if (address >= PRG_ROM_START_ADDRESS && address <= PRG_ROM_END_ADDRESS)
    {
        return mPrgRom->readByte(address - PRG_ROM_START_ADDRESS);
    }

    // Address not mapped to cartridge
    return 0xFF;
}

void NesCatridge::writeByte(uint16_t address, uint8_t data)
{
    if (address >= EXPANSION_ROM_START_ADDRESS && address <= EXPANSION_ROM_END_ADDRESS)
    {
        mExpansionRom->writeByte(address - EXPANSION_ROM_START_ADDRESS, data);
    }
    else if (address >= SRAM_ROM_START_ADDRESS && address <= SRAM_ROM_END_ADDRESS)
    {
        mSram->writeByte(address - SRAM_ROM_START_ADDRESS, data);
    }
    // PRG-ROM is read-only; ignore writes or handle via mapper
}