#pragma once

#include "IBus.h"
#include "Memory/Memory.h"
#include <memory>

class Bus : public IBus
{
    
public:
    Bus();
    ~Bus();

    uint8_t readByte(uint32_t address);
    uint16_t readWord(uint32_t address);
    void writeByte(uint32_t address, uint8_t data);
    void writeWord(uint32_t address, uint16_t data);

    void connectMemory(std::shared_ptr<Memory> memory);

private:
    std::shared_ptr<Memory> mMemory;
    
};
