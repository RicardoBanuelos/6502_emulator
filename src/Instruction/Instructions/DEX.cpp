#include "Instruction/Instructions/DEX.h"

DEX::DEX(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("DEX", icpu, addressingMode, cycles)
{
}

DEX::~DEX()
{
}

void DEX::run()
{
    uint8_t X = mIcpu->getRegister(Register::X);
    mIcpu->setRegister(Register::X, --X);

    mIcpu->setFlag(Flag::Z, X == 0);
    mIcpu->setFlag(Flag::N, X & 0x80);
}
