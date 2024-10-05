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
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    
    if(mIcpu->getFlag(Flag::N))
        return;
        
    uint16_t result = mIcpu->getRegister(Register::PC) + static_cast<int8_t>(addressingData.address);
    mIcpu->setRegister(Register::PC, result);
}
