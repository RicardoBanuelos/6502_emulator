#include "Instruction/Instructions/CLC.h"

CLC::CLC(std::shared_ptr<ICPU> icpu)
    :   Instruction("CLC", icpu, AddressingMode::Implied, 2)
{
}

CLC::~CLC()
{
}

void CLC::run()
{
    mIcpu->setFlag(Flag::C, 0);
}
