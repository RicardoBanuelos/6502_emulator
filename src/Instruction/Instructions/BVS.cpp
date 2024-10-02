#include "Instruction/Instructions/BVS.h"

BVS::BVS(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("BVS", icpu, addressingMode, cycles)
{
}

BVS::~BVS()
{
}

void BVS::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    
    if(!mIcpu->getFlag(Flag::V))
        return;
        
    uint16_t result = mIcpu->getRegister(Register::PC) + addressingData.address;
    mIcpu->setRegister(Register::PC, result);
}
