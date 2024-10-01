#pragma once 

#include <stdint.h>
#include <functional>
#include "Registers.h"
#include "AddressingMode.h"
#include "AddressingData.h"

class Addressing;

class ICPU
{

public:
    virtual ~ICPU();

    
protected:
    ICPU();
public:
    virtual uint8_t fetchByte() = 0;
    virtual uint16_t fetchWord() = 0;
    
    virtual void writeByte(uint16_t address, uint8_t byte) = 0;
    virtual void writeWord(uint16_t address, uint16_t word) = 0;
    virtual uint8_t readByte(uint16_t address) = 0;
    virtual uint16_t readWord(uint16_t address) = 0;
    
    virtual void pushByte(uint8_t data) = 0;
    virtual uint8_t popByte() = 0;
    virtual void pushWord(uint16_t data) = 0;
    virtual uint16_t popWord() = 0;
    
    virtual AddressingData addressing(AddressingMode mode) const = 0;
    
    virtual void setFlag(Flag flag, bool value) = 0;
    virtual bool getFlag(Flag) = 0;
    virtual void setRegister(Register reg, uint16_t value) = 0;
    virtual uint16_t getRegister(Register reg) = 0;

};
