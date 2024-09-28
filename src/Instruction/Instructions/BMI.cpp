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
    if(mIcpu->registers().status.N == 0)
        return;

    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    mIcpu->registers().PC += addressingData.address;
}
