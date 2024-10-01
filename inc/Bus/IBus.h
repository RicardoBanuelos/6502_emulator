#pragma once

#include <stdint.h>

class IBus
{
    
public:
    ~IBus(){};

    virtual uint8_t readByte(uint32_t address) = 0;
    virtual uint16_t readWord(uint32_t address) = 0;
    virtual void writeByte(uint32_t address, uint8_t data) = 0;
    virtual void writeWord(uint32_t address, uint16_t data) = 0;

protected:
    IBus(){};
};