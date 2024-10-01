#include "Instruction/Instructions/BPL.h"

BPL::BPL(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("BPL", icpu, addressingMode, cycles)
{
}

BPL::~BPL()
{
}

void BPL::run()
{
    if(mIcpu->getFlag(Flag::N))
        return;
        
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    uint16_t result = mIcpu->getRegister(Register::PC) + addressingData.address;
    mIcpu->setRegister(Register::PC, result);
}
