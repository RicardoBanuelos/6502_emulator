#pragma once

#include "IBus.h"
#include "Memory.h"
#include <memory>

class CPU;

class Bus : public IBus
{
    
public:
    Bus();
    ~Bus();

    virtual uint8_t readByte(uint32_t address);
    virtual uint16_t readWord(uint32_t address);
    virtual void writeByte(uint32_t address, uint8_t data);
    virtual void writeWord(uint32_t address, uint16_t data);

    virtual void connectMemory(std::shared_ptr<Memory> memory);

private:
    std::shared_ptr<CPU> mCPU;
    std::shared_ptr<Memory> mMemory;
};
