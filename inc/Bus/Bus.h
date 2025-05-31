#pragma once

#include "IBus.h"
#include "Memory.h"
#include <memory>

class Bus : public IBus
{
    
public:
    Bus();
    ~Bus();

    virtual uint8_t readByte(uint16_t address);
    virtual uint16_t readWord(uint16_t address);
    virtual void writeByte(uint16_t address, uint8_t data);
    virtual void writeWord(uint16_t address, uint16_t data);

    virtual void connectMemory(std::shared_ptr<Memory> memory);

protected:
    std::shared_ptr<Memory> mMemory;

};
