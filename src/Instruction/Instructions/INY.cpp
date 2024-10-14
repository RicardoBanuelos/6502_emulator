#include "Instruction/Instructions/INY.h"

INY::INY(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("INY", icpu, addressingMode, cycles)
{
}

INY::~INY()
{
}

void INY::run()
{
    uint8_t Y = mIcpu->getRegister(Register::Y);
    uint8_t result = Y + 1;

    mIcpu->setRegister(Register::Y, result);
    mIcpu->setFlag(Flag::Z, result == 0);
    mIcpu->setFlag(Flag::N, result & 0x80);
}
