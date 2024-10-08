#include "Instruction/Instructions/INX.h"

INX::INX(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("INX", icpu, addressingMode, cycles)
{
}

INX::~INX()
{
}

void INX::run()
{
    uint8_t X = mIcpu->getRegister(Register::X);
    uint8_t result = X + 1;

    mIcpu->setRegister(Register::X, result);
    mIcpu->setFlag(Flag::Z, result == 0);
    mIcpu->setFlag(Flag::N, result & 0x80);
}
