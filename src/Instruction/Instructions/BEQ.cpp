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

    if(!mIcpu->getFlag(Flag::Z))
        return;
        
    uint16_t result = mIcpu->getRegister(Register::PC) + addressingData.address;
    mIcpu->setRegister(Register::PC, result);
}
