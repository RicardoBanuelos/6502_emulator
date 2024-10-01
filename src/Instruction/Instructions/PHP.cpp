#include "Instruction/Instructions/PHP.h"

PHP::PHP(std::shared_ptr<ICPU> icpu, uint8_t cycles)
    : Instruction("PHP", icpu, AddressingMode::NA, cycles)
{
}

PHP::~PHP()
{
}

void PHP::run()
{
    //mIcpu->pushByte(mIcpu->registers().statusRegister.byte);
}