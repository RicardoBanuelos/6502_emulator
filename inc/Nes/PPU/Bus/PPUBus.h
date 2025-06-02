#pragma once
#include <memory>
#include "IBus.h"
#include "ICatridge.h"

/**
    $0000–$1FFF	Pattern tables	Cartridge (CHR-ROM/RAM)
    $2000–$2FFF	Nametables	PPU VRAM (or cartridge)
    $3000–$3EFF	Nametable mirrors	PPU VRAM (or cartridge)
    $3F00–$3FFF	Palettes	PPU internal palette RAM
 */

class PPUBus : public IBus
{

public:
    PPUBus();
    ~PPUBus();

    virtual uint8_t readByte(uint16_t address) override;
    virtual uint16_t readWord(uint16_t address) override;
    virtual void writeByte(uint16_t address, uint8_t data) override;
    virtual void writeWord(uint16_t address, uint16_t data) override;

    void connectCatridge(std::shared_ptr<ICatridge> catridge);

private:
    constexpr static uint16_t PATTERN_TABLE_START_ADDRESS = 0x0000;
    constexpr static uint16_t PATTERN_TABLE_END_ADDRESS = 0x1FFF;
    constexpr static uint16_t NAMETABLE_START_ADDRESS = 0x2000;
    constexpr static uint16_t NAMETABLE_END_ADDRESS = 0x2FFF;
    constexpr static uint16_t NAMETABLE_MIRROR_START_ADDRESS = 0x3000;
    constexpr static uint16_t NAMETABLE_MIRROR_END_ADDRESS = 0x3EFF;
    constexpr static uint16_t PALETTE_START_ADDRESS = 0x3F00;
    constexpr static uint16_t PALETTE_END_ADDRESS = 0x3FFF;


    constexpr static uint16_t PATTERN_TABLE_SIZE = 0x2000; // 8 KB
    constexpr static uint16_t NAMETABLE_SIZE = 0x400; // 1 KB
    constexpr static uint16_t PALETTE_SIZE = 0x20; // 32 bytes

    std::shared_ptr<ICatridge> mCatridge; // Pointer to the cartridge for CHR-ROM/RAM access
};
