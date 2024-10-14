#include "TAY.h"

TAY::TAY(std::shared_ptr<ICPU> icpu)
    :   Instruction("TAY", icpu, AddressingMode::Implied, 2)
{
}

TAY::~TAY()
{
}

void TAY::run()
{
    mIcpu->setRegister(Register::Y, mIcpu->getRegister(Register::A));
    
    uint8_t reg = mIcpu->getRegister(Register::Y);

    mIcpu->setFlag(Flag::Z, reg == 0);
    mIcpu->setFlag(Flag::N, reg & Flag::N);
}
