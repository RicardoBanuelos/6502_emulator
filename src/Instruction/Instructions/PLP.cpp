#include "PLP.h"


PLP::PLP(std::shared_ptr<ICPU> icpu, uint8_t cycles)
    :   Instruction("PLP", icpu, AddressingMode::Implied, cycles)
{
}

PLP::~PLP()
{
}

void PLP::run()
{
    // Pull status from stack
    uint8_t pulledPS = mIcpu->popByte();
    
    // Get current PS value to preserve B flag
    uint8_t currentPS = mIcpu->getRegister(Register::PS);
    
    // Extract B flag from current PS
    uint8_t bFlag = currentPS & 0x10;
    
    // Create new PS:
    // - Keep the original B flag (0x10)
    // - Always set bit 5 (0x20)
    // - Take all other bits from the pulled value
    uint8_t newPS = (pulledPS & 0xEF) | bFlag | 0x20;
    
    // Set the processor status register
    mIcpu->setRegister(Register::PS, newPS);
}