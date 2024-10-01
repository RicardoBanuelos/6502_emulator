#include "Instruction/Instructions/PLP.h"


PLP::PLP(std::shared_ptr<ICPU> icpu, uint8_t cycles)
    :   Instruction("PLP", icpu, AddressingMode::Implied, cycles)
{
}

PLP::~PLP()
{
}

void PLP::run()
{
    //mIcpu->registers().statusRegister.byte = mIcpu->popByte();
}