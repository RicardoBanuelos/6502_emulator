#include "Instruction/Instructions/CLC.h"

CLC::CLC(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("CLC", icpu, addressingMode, cycles)
{
}

CLC::~CLC()
{
}

void CLC::run()
{
    mIcpu->setFlag(Flag::C, 0);
}
