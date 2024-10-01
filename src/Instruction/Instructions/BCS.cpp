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
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    
    if(!mIcpu->getFlag(Flag::C))
        return;

    uint16_t result = mIcpu->getRegister(Register::PC) + addressingData.address;
    mIcpu->setRegister(Register::PC, result);
}
