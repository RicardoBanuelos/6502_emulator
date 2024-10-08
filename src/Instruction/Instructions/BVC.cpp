#include "Instruction/Instructions/BVC.h"

BVC::BVC(std::shared_ptr<ICPU> icpu)
    :   Instruction("BVC", icpu, AddressingMode::Relative, 2)
{
}

BVC::~BVC()
{
}

void BVC::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    
    if(mIcpu->getFlag(Flag::V))
        return;
        
    uint16_t result = mIcpu->getRegister(Register::PC) + static_cast<int8_t>(addressingData.address);
    mIcpu->setRegister(Register::PC, result);
}
