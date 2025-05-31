#include "BRK.h"

BRK::BRK(std::shared_ptr<ICPU> icpu)
    : Instruction("BRK", icpu, AddressingMode::Implied, 7)
{
}

BRK::~BRK()
{
}

void BRK::run()
{
    // PC+2 accounts for opcode byte and padding byte
    uint16_t pc = mIcpu->getRegister(Register::PC) + 2;
    
    // Push PC+2 to stack (high byte first)
    mIcpu->pushWord(pc);
    
    // Push status register with B flag and bit 5 set
    uint8_t ps = mIcpu->getRegister(Register::PS);
    ps |= Flag::B;  // Set B flag in the stack value
    ps |= 0x20;     // Set bit 5 (always 1 in stack)
    mIcpu->pushByte(ps);
    
    // Do NOT set I flag (differs from hardware IRQ)
    // According to specification: "The interrupt disable flag is not set automatically."
    
    
    // Load IRQ vector into PC
    uint16_t IRQ_interrupt_vector = mIcpu->readWord(0xFFFE);
    mIcpu->setRegister(Register::PC, IRQ_interrupt_vector);
}
