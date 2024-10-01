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
    if(mIcpu->getFlag(Flag::C))
        return;

    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    uint16_t result = mIcpu->getRegister(Register::PC) + addressingData.address;
    mIcpu->setRegister(Register::PC, result);
}
