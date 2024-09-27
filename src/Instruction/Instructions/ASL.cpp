#include "Instruction/Instructions/ASL.h"

ASL::ASL(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("ASL", icpu, addressingMode, cycles)
{
}

ASL::~ASL()
{
}

void ASL::run()
{
    uint16_t address = 0;
    uint16_t fetched = 0;
    uint16_t result = 0;

    if(mAddressingMode == AddressingMode::Implied)
    {
        fetched = mIcpu->addressing(mAddressingMode);
        result = fetched << 1;

        mIcpu->registers().A = result & 0x00FF;
    }
    else
    {
        address = mIcpu->addressing(mAddressingMode);
        fetched = mIcpu->memory().readByte(address);
        result = fetched << 1;
        
        mIcpu->writeByte(address, result & 0x00FF);
    }   

    mIcpu->registers().status.C = result & 0xFF00 > 0;
    mIcpu->registers().status.Z = result & 0x00FF == 0;
    mIcpu->registers().status.N = result & 0x80 == 0;
}
