#include "Instruction/Instructions/LDY.h"

LDY::LDY(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("LDY", icpu, addressingMode, cycles)
{
}

LDY::~LDY()
{
}

void LDY::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    mIcpu->setRegister(Register::Y, addressingData.data);
    mIcpu->setFlag(Flag::Z, mIcpu->getRegister(Register::Y) == 0);
    mIcpu->setFlag(Flag::N, mIcpu->getRegister(Register::Y) & Flag::N);
}

