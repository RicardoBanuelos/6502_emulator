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
    mIcpu->setRegister(Register::A, mIcpu->popByte());

    mIcpu->setFlag(Flag::Z, mIcpu->getRegister(Register::A) == 0);
    mIcpu->setFlag(Flag::N, mIcpu->getRegister(Register::A) & Flag::N);    
}
