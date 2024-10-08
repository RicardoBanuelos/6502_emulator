#include "TXA.h"

TXA::TXA(std::shared_ptr<ICPU> icpu)
    :   Instruction("TXA", icpu, AddressingMode::Implied, 2)
{
}

TXA::~TXA()
{
}

void TXA::run()
{
    mIcpu->setRegister(Register::A, mIcpu->getRegister(Register::X));
    
    uint8_t reg = mIcpu->getRegister(Register::A);

    mIcpu->setFlag(Flag::Z, reg == 0);
    mIcpu->setFlag(Flag::N, reg & Flag::N);
}
