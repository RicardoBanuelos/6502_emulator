#pragma once

#include "Registers.h"
#include "IPPU.h"


/*

PPU Address Space ($0000–$3FFF)
$0000–$1FFF: Pattern Tables (CHR-ROM/CHR-RAM, from cartridge)
$2000–$2FFF: Nametables (background tile maps, internal or cartridge RAM)
$3000–$3EFF: Mirrors of $2000–$2EFF
$3F00–$3FFF: Palettes (background/sprite colors, mirrored every 32 bytes)

*/
class PPU : public IPPU
{

public:
    PPU();
    ~PPU();


    void writeRegister(uint16_t addr, uint8_t data) override;
    uint8_t readRegister(uint16_t addr) const override;
    

private:
    PPURegisters mRegisters;

};
