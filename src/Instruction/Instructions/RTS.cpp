#include "Instruction/Instructions/RTS.h"


RTS::RTS(std::shared_ptr<ICPU> icpu)
    :   Instruction("RTS", icpu, AddressingMode::Implied, 6)
{
}

RTS::~RTS()
{
}

void RTS::run()
{
    // mIcpu->registers().PC = mIcpu->popWord();
}
