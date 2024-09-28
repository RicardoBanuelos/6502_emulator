#include "Instruction/Instructions/BNE.h"

BNE::BNE(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("BNE", icpu, addressingMode, cycles)
{
}

BNE::~BNE()
{
}

void BNE::run()
{
    if(mIcpu->registers().status.Z)
        return;
        
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    mIcpu->registers().PC + addressingData.address;
}
