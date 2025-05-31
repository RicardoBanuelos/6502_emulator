#include "PHP.h"

PHP::PHP(std::shared_ptr<ICPU> icpu, uint8_t cycles)
    : Instruction("PHP", icpu, AddressingMode::Implied, cycles)
{
}

PHP::~PHP()
{
}

void PHP::run()
{
    // Get processor status
    uint8_t ps = mIcpu->getRegister(Register::PS);
    
    // Set B flag (bit 4) and unused bit (bit 5) in the value to be pushed
    ps |= 0x30;
    
    // Push modified value onto stack
    mIcpu->pushByte(ps);
}