#pragma once 

#include <stdint.h>
#include <functional>
#include "Registers.h"
#include "Memory/Memory.h"
#include "AddressingMode.h"
#include "AddressingData.h"

class Addressing;

class ICPU
{

public:
    virtual ~ICPU(){};

    
protected:
    ICPU(){};
public:
    virtual uint8_t fetchByte() = 0;
    virtual uint16_t fetchWord() = 0;
    
    virtual void writeByte(uint16_t address, uint8_t byte) = 0;
    virtual void writeWord(uint16_t address, uint16_t word) = 0;
    
    virtual void pushByte(uint8_t data) = 0;
    virtual uint8_t popByte() = 0;
    virtual void pushWord(uint16_t data) = 0;
    virtual uint16_t popWord() = 0;
    
    virtual const AddressingData addressing(AddressingMode mode) const = 0;
    
    virtual Registers &registers() = 0;
    virtual Memory &memory() = 0;
};
