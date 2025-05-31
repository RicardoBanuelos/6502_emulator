#pragma once 

#include "Bus.h"
#include "IPPU.h"

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


class NesBus : public Bus
{
    
public:
    NesBus();
    ~NesBus();


    uint8_t readByte(uint16_t address) override;
    uint16_t readWord(uint16_t address) override;
    void writeByte(uint16_t address, uint8_t data) override;
    void writeWord(uint16_t address, uint16_t data) override;
    void connectPPU(std::shared_ptr<IPPU> ppu);
private:

    std::shared_ptr<IPPU> mPPU;
    uint16_t mapAddress(uint16_t address) const;

    static const uint16_t RAM_LAST_ADDRESS = 0x1FFF;
    static const uint16_t PPU_LAST_ADDRESS = 0x3FFF;
    static const uint16_t APU_IO_LAST_ADDRESS = 0x401F;

    static const uint16_t RAM_LAST_MIRRORED_ADDRESS = 0x07FF;
    static const uint16_t PPU_LAST_MIRRORED_ADDRESS = 0x2007;
    

};
