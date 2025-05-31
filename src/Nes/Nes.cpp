#include "Nes.h"

Nes::Nes()
    : mBus(std::make_shared<NesBus>()),
      mMemory(std::make_shared<Memory>()),
      mPPU(std::make_shared<PPU>()),
      mCPU(std::make_shared<CPU>())
{
    mMemory->initialize();
    mCPU->init();
    
    mBus->connectMemory(mMemory);
    mBus->connectPPU(mPPU);
    


    mCPU->connectBus(mBus);
}

Nes::~Nes()
{
}

#ifdef UNIT_TEST


    void Nes::writeByte(uint32_t address, uint8_t data)
    {
        mBus->writeByte(address, data);
    }

    uint8_t Nes::readByte(uint32_t address) const
    {
        return mBus->readByte(address);
    }
    void Nes::writeWord(uint32_t address, uint16_t data)
    {
        mBus->writeWord(address, data);
    }
    uint16_t Nes::readWord(uint32_t address) const
    {
        return mBus->readWord(address);
    }
#endif