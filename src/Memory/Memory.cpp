#include "Memory/Memory.h"
#include "Memory.h"

Memory::Memory(){}
Memory::~Memory(){}

void Memory::initialize()
{
    for(uint32_t i = 0; i < MAX_MEMORY; ++i)
    {
        data[i] = 0;
    }
}

uint8_t Memory::readByte(uint32_t address)
{
    if(!validateAddress(address)) 
        return 0;

    return data[address];
}

uint16_t Memory::readWord(uint32_t address)
{
    if(!validateAddress(address)) 
        return 0;

    uint16_t low = data[address];
    uint16_t high = data[address + 1] << 8;

    return high | low;
}

void Memory::writeByte(uint32_t address)
{
    if(!validateAddress(address)) 
        return;
}

void Memory::writeWord(uint32_t address)
{
    if(!validateAddress(address)) 
        return;
}

bool Memory::validateAddress(uint32_t address)
{
    return address >= 0 && address < MAX_MEMORY;
}
