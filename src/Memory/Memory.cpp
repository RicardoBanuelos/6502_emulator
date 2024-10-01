#include "Memory/Memory.h"
#include "Memory.h"

Memory::Memory(){}
Memory::~Memory(){}

void Memory::initialize()
{
    for(uint32_t i = 0; i < MAX_MEMORY; ++i)
    {
        mData[i] = 0;
    }
}

uint8_t Memory::readByte(uint32_t address) const
{
    if(!validateAddress(address)) 
        return 0;

    return mData[address];
}

uint16_t Memory::readWord(uint32_t address) const
{
    if(!validateAddress(address + 1)) 
        return 0;

    uint16_t tmp;
    uint8_t low = mData[address];
    uint8_t high = mData[address + 1];

    tmp = high;
    tmp <<= 8;
    tmp |= low;

    return tmp;
}

void Memory::writeByte(uint32_t address, uint8_t byte)
{
    if(!validateAddress(address)) 
        return;

    mData[address] = byte;
}

void Memory::writeWord(uint32_t address, uint16_t word)
{
    if(!validateAddress(address + 1)) 
        return;

    mData[address] = word & 0x00FF;
    mData[address + 1] = (word >> 8);
}

bool Memory::validateAddress(uint32_t address) const
{
    return address >= 0 && address < MAX_MEMORY;
}
