#include "Instruction/Instructions/PHA.h"


PHA::PHA(std::shared_ptr<ICPU> icpu, uint8_t cycles)
    :   Instruction("PHA", icpu, AddressingMode::Implied, cycles)
{
}

PHA::~PHA()
{
}

void PHA::run()
{
    mIcpu->pushByte(mIcpu->getRegister(Register::A));
}