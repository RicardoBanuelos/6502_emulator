#include "Instruction/Instructions/BVC.h"

BVC::BVC(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("BVC", icpu, addressingMode, cycles)
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
        
    uint16_t result = mIcpu->getRegister(Register::PC) + addressingData.address;
    mIcpu->setRegister(Register::PC, result);
}
