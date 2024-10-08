#include "Memory/Memory.h"
#include <random>
#include <fstream>

Memory::Memory(){}
Memory::~Memory(){}

void Memory::initialize()
{
    for(uint32_t i = 0; i < MAX_MEMORY; ++i)
    {
        mData[i] = 0;
    }
}

void Memory::randomize()
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis(0, UINT8_MAX);

    for(int i = 0; i < MAX_MEMORY; ++i)
    {
        mData[i] = dis(gen);
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

bool Memory::loadBinary(const std::string &path, uint16_t startAddress)
{
    std::ifstream binaryFile(path, std::ios::in | std::ios::binary);
    if(!binaryFile.is_open())
    {
        
        return false;
    }


    char buffer;
    while(binaryFile.read(&buffer, sizeof(char)))
    {
        mData[startAddress++] = static_cast<uint8_t>(buffer);
    }

    return true;
}

bool Memory::validateAddress(uint32_t address) const
{
    return address >= 0 && address < MAX_MEMORY;
}
