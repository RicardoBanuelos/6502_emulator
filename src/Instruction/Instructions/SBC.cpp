#include "Instruction/Instructions/SBC.h"

SBC::SBC(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("SBC", icpu, addressingMode, cycles)
{
}

SBC::~SBC()
{
}

void SBC::run()
{
    uint16_t value = mIcpu->addressing(mAddressingMode).data;
    uint16_t accumulator = mIcpu->getRegister(Register::A); 
    uint16_t carry = mIcpu->getFlag(Flag::C); 
    
    uint16_t tmp = accumulator - value - carry;

    mIcpu->setFlag(Flag::C, tmp & 0xFF00);
    mIcpu->setFlag(Flag::Z, tmp == 0);
    mIcpu->setFlag(Flag::V, ((tmp ^ accumulator) & (tmp ^ value) & 0x0080));
    mIcpu->setFlag(Flag::N, tmp & Flag::N);
    
    mIcpu->setRegister(Register::A, tmp & 0x00FF);
}
