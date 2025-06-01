#pragma once 
#include <memory>
#include "Memory.h"
#include "ICatridge.h"

class NesCatridge : public ICatridge
{
    
public:
    NesCatridge();
    ~NesCatridge();


    uint8_t readByte(uint16_t address) const override;
    void writeByte(uint16_t address, uint8_t data) override;


private:
    constexpr static uint16_t EXPANSION_ROM_START_ADDRESS = 0x4020;
    constexpr static uint16_t SRAM_ROM_START_ADDRESS = 0x6000;
    constexpr static uint16_t PRG_ROM_START_ADDRESS = 0x8000;
    constexpr static uint16_t EXPANSION_ROM_END_ADDRESS = 0x5FFF;
    constexpr static uint16_t SRAM_ROM_END_ADDRESS = 0x7FFF;
    constexpr static uint16_t PRG_ROM_END_ADDRESS = 0xFFFF;

    std::unique_ptr<Memory> mExpansionRom;	
    std::unique_ptr<Memory> mSram;	
    std::unique_ptr<Memory> mPrgRom;
};
