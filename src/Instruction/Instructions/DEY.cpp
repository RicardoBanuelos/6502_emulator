#include "Instruction/Instructions/DEY.h"

DEY::DEY(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("DEX", icpu, addressingMode, cycles)
{
}

DEY::~DEY()
{
}

void DEY::run()
{
    uint8_t Y = mIcpu->getRegister(Register::Y);
    mIcpu->setRegister(Register::Y, --Y);

    mIcpu->setFlag(Flag::Z, Y == 0);
    mIcpu->setFlag(Flag::N, Y & 0x80);
}
