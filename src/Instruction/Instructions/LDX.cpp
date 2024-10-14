#include "Instruction/Instructions/LDX.h"


LDX::LDX(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("LDX", icpu, addressingMode, cycles)
{
}

LDX::~LDX()
{
    
}

void LDX::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);

    mIcpu->setRegister(Register::X, addressingData.data);
    mIcpu->setFlag(Flag::Z, mIcpu->getRegister(Register::X) == 0);
    mIcpu->setFlag(Flag::N, mIcpu->getRegister(Register::X) & Flag::N);
}
