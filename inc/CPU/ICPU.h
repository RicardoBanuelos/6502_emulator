#pragma once 

#include <stdint.h>
#include "Registers.h"
#include "Memory/Memory.h"

class ICPU
{
    
protected:
    ICPU(){};
    virtual ~ICPU(){};
public:
    virtual uint8_t fetchByte() = 0;
    virtual uint16_t fetchWord() = 0;
    virtual void writeByte(uint8_t address, uint8_t byte) = 0;
    virtual void writeWord(uint8_t address, uint16_t word) = 0;

    virtual Registers &registers() = 0;
    virtual const Memory &memory() = 0;
};
