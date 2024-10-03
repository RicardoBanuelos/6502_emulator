#include "TAX.h"

TAX::TAX(std::shared_ptr<ICPU> icpu)
    :   Instruction("TAX", icpu, AddressingMode::Implied, 2)
{
}

TAX::~TAX()
{
}

void TAX::run()
{
    mIcpu->setRegister(Register::X, mIcpu->getRegister(Register::A));
    
    uint8_t reg = mIcpu->getRegister(Register::X);

    mIcpu->setFlag(Flag::Z, reg == 0);
    mIcpu->setFlag(Flag::N, reg & Flag::N);
}
