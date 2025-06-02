#pragma once

#include <cstdint>


class ICatridge
{
public:
    virtual uint8_t readByte(uint16_t address) const = 0;
    virtual void writeByte(uint16_t address, uint8_t data) = 0;
    virtual bool loadFromFile(const std::string& filepath) = 0;

protected:
    ICatridge() = default;
    ~ICatridge() = default;

};
