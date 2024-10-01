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
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    
    if(mIcpu->getFlag(Flag::Z))
        return;
        
    uint16_t result = mIcpu->getRegister(Register::PC) + addressingData.address;
    mIcpu->setRegister(Register::PC, result);
}
