#pragma once

#include <cstdint>

class IPPU
{

public:
    IPPU() = default;
    ~IPPU() = default;

    virtual void writeRegister(uint32_t addr, uint8_t data) = 0;
    virtual uint8_t readRegister(uint32_t addr) const = 0;

private:
};