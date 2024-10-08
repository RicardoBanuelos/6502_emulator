#include "Bus.h"

Bus::Bus()
{
}

Bus::~Bus()
{
}

uint8_t Bus::readByte(uint32_t address)
{
    return mMemory->readByte(address);
}

uint16_t Bus::readWord(uint32_t address)
{
    return mMemory->readWord(address);
}

void Bus::writeByte(uint32_t address, uint8_t data)
{
    mMemory->writeByte(address, data);
}

void Bus::writeWord(uint32_t address, uint16_t data)
{
    mMemory->writeWord(address, data);
}

void Bus::connectMemory(std::shared_ptr<Memory> memory)
{
    mMemory = memory;
}
