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
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    uint16_t result = addressingData.data << 1;

    mIcpu->registers().status.C = result & 0xFF00 > 0;
    mIcpu->registers().status.Z = result & 0x00FF == 0;
    mIcpu->registers().status.N = result & 0x80 == 0;

    if(mAddressingMode == AddressingMode::Implied)
    {
        mIcpu->registers().A = result & 0x00FF;
    }
    else
    {
        mIcpu->writeByte(addressingData.address, result & 0x00FF);
    }
}
