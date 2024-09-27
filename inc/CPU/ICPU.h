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
    virtual void writeByte(uint8_t data) = 0;
    virtual void writeWord(uint16_t data) = 0;

    virtual const Registers &registers() = 0;
    virtual const Memory &memory() = 0;
};
