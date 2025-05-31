#pragma once

#include <memory>
#include "NesBus.h"
#include "CPU.h"
#include "PPU.h"

class Nes
{

public:
    Nes();
    ~Nes();


#ifdef UNIT_TEST

    public:
        void writeByte(uint32_t address, uint8_t data);
        void writeWord(uint32_t address, uint8_t data);
        uint8_t readByte(uint32_t address) const;
        uint16_t readWord(uint32_t address) const;
#endif

private:
    std::shared_ptr<Memory> mMemory;
    std::shared_ptr<PPU> mPPU;
    std::shared_ptr<CPU> mCPU;
    std::shared_ptr<NesBus> mBus;

};
