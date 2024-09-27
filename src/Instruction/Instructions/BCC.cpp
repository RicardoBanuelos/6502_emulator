#include "Instruction/Instructions/BCC.h"

BCC::BCC(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("BCC", icpu, addressingMode, cycles)
{
}

BCC::~BCC()
{
}

void BCC::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);

    if(mIcpu->registers().status.C)
        return;

    mIcpu->registers().PC += addressingData.address;
}
