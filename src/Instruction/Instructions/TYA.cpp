#include "TYA.h"

TYA::TYA(std::shared_ptr<ICPU> icpu)
    :   Instruction("TYA", icpu, AddressingMode::Implied, 2)
{
}

TYA::~TYA()
{
}

void TYA::run()
{
    mIcpu->setRegister(Register::A, mIcpu->getRegister(Register::Y));


    uint8_t reg = mIcpu->getRegister(Register::A);

    mIcpu->setFlag(Flag::Z, reg == 0);
    mIcpu->setFlag(Flag::N, reg & Flag::N);
}
