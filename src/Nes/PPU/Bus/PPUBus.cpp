#include "PPUBus.h"

// You will need to add pointers/references to CHR-ROM/RAM, VRAM, and palette RAM in the class for a full implementation.

PPUBus::PPUBus()
{
    // TODO: Initialize pointers to CHR-ROM/RAM, VRAM, and palette RAM here
}

PPUBus::~PPUBus()
{
}

uint8_t PPUBus::readByte(uint16_t address)
{
    address &= 0x3FFF; // PPU address space wraps every 16KB

    if (address >= PATTERN_TABLE_START_ADDRESS && address <= PATTERN_TABLE_END_ADDRESS)
    {
        // Read from CHR-ROM/CHR-RAM (pattern tables) on the cartridge
        // return chrRomOrRam->readByte(address);
        // TODO: Implement or connect to cartridge
        return mCatridge ->readByte(address); 
    }
    else if (address >= NAMETABLE_START_ADDRESS && address <= NAMETABLE_END_ADDRESS)
    {
        // Read from PPU VRAM (nametables)
        // return vram->readByte(address - NAMETABLE_START_ADDRESS);
        // TODO: Implement or connect to VRAM
        return 0x00;
    }
    else if (address >= NAMETABLE_MIRROR_START_ADDRESS && address <= NAMETABLE_MIRROR_END_ADDRESS)
    {
        // Mirror of $2000–$2EFF (nametables)
        // uint16_t mirrored = (address - 0x1000);
        // return vram->readByte(mirrored - NAMETABLE_START_ADDRESS);
        // TODO: Implement mirroring logic
        return 0x00;
    }
    else if (address >= PALETTE_START_ADDRESS && address <= PALETTE_END_ADDRESS)
    {
        // Read from PPU internal palette RAM (32 bytes, mirrored)
        // return paletteRam->readByte(address & 0x1F);
        // TODO: Implement or connect to palette RAM
        return 0x00;
    }

    // Unmapped area
    return 0xFF;
}

uint16_t PPUBus::readWord(uint16_t address)
{
    // PPU is little-endian: low byte first, then high byte
    uint8_t low = readByte(address);
    uint8_t high = readByte(address + 1);
    return (high << 8) | low;
}

void PPUBus::writeByte(uint16_t address, uint8_t data)
{
    address &= 0x3FFF; // PPU address space wraps every 16KB

    if (address >= PATTERN_TABLE_START_ADDRESS && address <= PATTERN_TABLE_END_ADDRESS)
    {
        // Write to CHR-RAM (if present, not CHR-ROM)
        // chrRam->writeByte(address, data);
        // TODO: Implement or connect to CHR-RAM
    }
    else if (address >= NAMETABLE_START_ADDRESS && address <= NAMETABLE_END_ADDRESS)
    {
        // Write to PPU VRAM (nametables)
        // vram->writeByte(address - NAMETABLE_START_ADDRESS, data);
        // TODO: Implement or connect to VRAM
    }
    else if (address >= NAMETABLE_MIRROR_START_ADDRESS && address <= NAMETABLE_MIRROR_END_ADDRESS)
    {
        // Mirror of $2000–$2EFF (nametables)
        // uint16_t mirrored = (address - 0x1000);
        // vram->writeByte(mirrored - NAMETABLE_START_ADDRESS, data);
        // TODO: Implement mirroring logic
    }
    else if (address >= PALETTE_START_ADDRESS && address <= PALETTE_END_ADDRESS)
    {
        // Write to PPU internal palette RAM (32 bytes, mirrored)
        // paletteRam->writeByte(address & 0x1F, data);
        // TODO: Implement or connect to palette RAM
    }
    // Unmapped area: do nothing
}

void PPUBus::writeWord(uint16_t address, uint16_t data)
{
    // Write low byte, then high byte (little-endian)
    writeByte(address, data & 0xFF);
    writeByte(address + 1, (data >> 8) & 0xFF);
}

void PPUBus::connectCatridge(std::shared_ptr<ICatridge> catridge)
{
    mCatridge = catridge;
}
