#include "Instruction/Instructions/BEQ.h"

BEQ::BEQ(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("BEQ", icpu, addressingMode, cycles)
{
}

BEQ::~BEQ()
{
}

void BEQ::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);

    if(mIcpu->registers().status.Z == 0)
        return;

    mIcpu->registers().PC += addressingData.address;
}
