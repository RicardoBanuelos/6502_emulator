#include "Instruction/Instructions/BCS.h"

BCS::BCS(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("BCS", icpu, addressingMode, cycles)
{
}

BCS::~BCS()
{
}

void BCS::run()
{
    if(mIcpu->registers().status.C == 0)
        return;

    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    mIcpu->registers().PC += addressingData.address;
}
