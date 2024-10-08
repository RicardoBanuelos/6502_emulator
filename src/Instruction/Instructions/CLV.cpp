#include "Instruction/Instructions/CLV.h"

CLV::CLV(std::shared_ptr<ICPU> icpu)
    :   Instruction("CLV", icpu, AddressingMode::Implied, 2)
{
}

CLV::~CLV()
{
}

void CLV::run()
{
    mIcpu->setFlag(Flag::V, 0);
}
