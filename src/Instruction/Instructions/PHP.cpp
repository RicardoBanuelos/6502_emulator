#include "Instructions/PHP.h"


PHP::PHP(std::shared_ptr<ICPU> icpu, uint8_t cycles)
    :   Instruction("PHP", icpu, AddressingMode::NA, cycles)
{
}

void PHP::run()
{
    mIcpu->pushByte(mIcpu->registers().status.byte);
}