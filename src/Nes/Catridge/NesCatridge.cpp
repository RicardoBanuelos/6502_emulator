#include "NesCatridge.h"
#include <fstream>

NesCatridge::NesCatridge()
    :   mExpansionRom(std::make_unique<Memory>(8 * 1024)), // 8 KB
        mSram(std::make_unique<Memory>(8 * 1024)),         // 8 KB
        mPrgRom(std::make_unique<Memory>(32 * 1024)),       // 32 KB
        mChrRom(std::make_unique<Memory>(8 * 1024))          // 8 KB
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

// Read and write methods for PPU address space (CHR-ROM)
uint8_t NesCatridge::readPPUByte(uint16_t ppuAddress) const
{
    if (ppuAddress >= CHR_ROM_START_ADDRESS && ppuAddress <= CHR_ROM_LAST_ADDRESS)
    {
        return mChrRom->readByte(ppuAddress - CHR_ROM_START_ADDRESS);
    }

    // Address not mapped to CHR-ROM
    return 0xFF;
}


// Write method for PPU address space (CHR-ROM)
void NesCatridge::writePPUByte(uint16_t ppuAddress, uint8_t data)
{
    if (ppuAddress >= CHR_ROM_START_ADDRESS && ppuAddress <= CHR_ROM_LAST_ADDRESS)
    {
        mChrRom->writeByte(ppuAddress - CHR_ROM_START_ADDRESS, data);
    }
    // CHR-ROM is read-only; ignore writes or handle via mapper
}


bool NesCatridge::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) return false;
    
    // Read iNES header (16 bytes)
    uint8_t header[16];
    file.read(reinterpret_cast<char*>(header), 16);
    
    // Validate header signature "NES" followed by MS-DOS EOF
    if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1A) {
        return false;
    }
    
    // Get PRG-ROM and CHR-ROM sizes
    uint8_t prgRomSize = header[4] * 16 * 1024; // 16KB units
    uint8_t chrRomSize = header[5] * 8 * 1024;  // 8KB units
    
    // Read PRG-ROM data
    std::vector<uint8_t> prgData(prgRomSize);
    file.read(reinterpret_cast<char*>(prgData.data()), prgRomSize);
    for (size_t i = 0; i < prgData.size(); i++) {
        mPrgRom->writeByte(i, prgData[i]);
    }
    
    // Read CHR-ROM data if present
    if (chrRomSize > 0) {
        std::vector<uint8_t> chrData(chrRomSize);
        file.read(reinterpret_cast<char*>(chrData.data()), chrRomSize);
        for (size_t i = 0; i < chrData.size(); i++) {
            mChrRom->writeByte(i, chrData[i]);
        }
    }
    
    return true;
}