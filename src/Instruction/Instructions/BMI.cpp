#include "Instruction/Instructions/BMI.h"

BMI::BMI(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("BMI", icpu, addressingMode, cycles)
{
}

BMI::~BMI()
{
}

void BMI::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    
    if(!mIcpu->getFlag(Flag::N))
        return;
        
    uint16_t result = mIcpu->getRegister(Register::PC) + static_cast<int8_t>(addressingData.address);
    mIcpu->setRegister(Register::PC, result);
}
