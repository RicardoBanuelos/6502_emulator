#pragma once 
#include <memory>
#include "Memory.h"
#include "Interfaces/ICatridge.h"

class NesCatridge : public ICatridge
{
    
public:
    NesCatridge();
    ~NesCatridge();

    bool loadFromFile(const std::string& filepath) override;

    uint8_t readByte(uint16_t address) const override;
    void writeByte(uint16_t address, uint8_t data) override;

    
    uint8_t readPPUByte(uint16_t ppuAddress) const;
    void writePPUByte(uint16_t ppuAddress, uint8_t data);

private:
    // Memory map for NES cartridge
    // $4020–$5FFF: Expansion ROM (8 KB)
    // $6000–$7FFF: SRAM (8 KB)
    // $8000–$FFFF: PRG-ROM (32 KB)
    constexpr static uint16_t CHR_ROM_START_ADDRESS = 0; 
    constexpr static uint16_t CHR_ROM_LAST_ADDRESS = 0x1FFF; 
    constexpr static uint16_t EXPANSION_ROM_START_ADDRESS = 0x4020;
    constexpr static uint16_t EXPANSION_ROM_END_ADDRESS = 0x5FFF;
    constexpr static uint16_t SRAM_ROM_START_ADDRESS = 0x6000;
    constexpr static uint16_t SRAM_ROM_END_ADDRESS = 0x7FFF;
    constexpr static uint16_t PRG_ROM_START_ADDRESS = 0x8000;
    constexpr static uint16_t PRG_ROM_END_ADDRESS = 0xFFFF;

    std::unique_ptr<Memory> mExpansionRom;	
    std::unique_ptr<Memory> mSram;	
    std::unique_ptr<Memory> mPrgRom;
    std::unique_ptr<Memory> mChrRom;

    //Mapper functionality can be added here if needed
};
