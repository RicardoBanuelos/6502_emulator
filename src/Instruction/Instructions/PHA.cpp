#include "Instruction/Instructions/PHA.h"


PHA::PHA(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("PHA", icpu, addressingMode, cycles)
{
}

PHA::~PHA()
{
}

void PHA::run()
{
    mIcpu->pushByte(mIcpu->registers().A);
}