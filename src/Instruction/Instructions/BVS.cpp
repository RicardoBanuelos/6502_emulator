#include "Instruction/Instructions/BVS.h"

BVS::BVS(std::shared_ptr<ICPU> icpu)
    :   Instruction("BVS", icpu, AddressingMode::Relative, 2)
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
        
    uint16_t result = mIcpu->getRegister(Register::PC) + static_cast<int8_t>(addressingData.address);
    mIcpu->setRegister(Register::PC, result);
}
