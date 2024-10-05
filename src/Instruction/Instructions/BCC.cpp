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
    
    if(mIcpu->getFlag(Flag::C))
        return;

    uint16_t result = mIcpu->getRegister(Register::PC) + static_cast<int8_t>(addressingData.address);
    mIcpu->setRegister(Register::PC, result);
}
