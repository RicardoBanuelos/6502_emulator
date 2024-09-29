#include "Instruction/Instructions/PLA.h"


PLA::PLA(std::shared_ptr<ICPU> icpu, uint8_t cycles)
    :   Instruction("PLA", icpu, AddressingMode::Implied, cycles)   
{
}

PLA::~PLA()
{
}

void PLA::run()
{
    mIcpu->registers().A = mIcpu->popByte();

    mIcpu->registers().status.setFlag(Flag::Z, mIcpu->registers().A == 0);
    mIcpu->registers().status.setFlag(Flag::N, mIcpu->registers().A & (1 << 7));
    
}
