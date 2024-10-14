#include "Instruction/Instructions/BRK.h"

BRK::BRK(std::shared_ptr<ICPU> icpu)
    :   Instruction("BRK", icpu, AddressingMode::Implied, 7)
{
}

BRK::~BRK()
{
}

void BRK::run()
{
    mIcpu->pushWord(mIcpu->getRegister(Register::PC));
    mIcpu->pushByte(mIcpu->getRegister(Register::PS));

    uint16_t IRQ_interrupt_vector = mIcpu->readWord(0xFFFE);
    mIcpu->setRegister(Register::PC, IRQ_interrupt_vector);
    mIcpu->setFlag(Flag::B, 1);
}
