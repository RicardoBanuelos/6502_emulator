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
    if(!mIcpu->getFlag(Flag::N))
        return;
        
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    uint16_t result = mIcpu->getRegister(Register::PC) + addressingData.address;
    mIcpu->setRegister(Register::PC, result);
}
