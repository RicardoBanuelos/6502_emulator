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
    if(mIcpu->registers().status.Z == 0)
        return;
        
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    mIcpu->registers().PC += addressingData.address;
}
