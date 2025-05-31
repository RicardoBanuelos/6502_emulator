#pragma once

#include <vector>
#include <cstdint>

class PPURegisters
{
public:
    enum RegistersIndex : uint8_t
    {
        PPUCTRL = 0x00,
        PPUMASK = 0x01,
        PPUSTATUS = 0x02,
        OAMADDR = 0x03,
        OAMDATA = 0x04,
        PPUSCROLL = 0x05,
        PPUADDR = 0x06,
        PPUDATA = 0x07,
        REGISTERS_SIZE
    };
    PPURegisters();
    ~PPURegisters();

    uint8_t read(RegistersIndex regIndex) const;
    void write(RegistersIndex regIndex, uint8_t value);

private:
    std::vector<uint8_t> reg;
};