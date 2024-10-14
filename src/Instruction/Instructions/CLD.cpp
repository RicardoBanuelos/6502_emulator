#include "Instruction/Instructions/CLD.h"

CLD::CLD(std::shared_ptr<ICPU> icpu)
    :   Instruction("CLD", icpu, AddressingMode::Implied, 2)
{
}

CLD::~CLD()
{
}

void CLD::run()
{
    mIcpu->setFlag(Flag::D, 0);
}
