#include "CLV.h"

CLV::CLV(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("CLV", icpu, addressingMode, cycles)
{
}

CLV::~CLV()
{
}

void CLV::run()
{
    mIcpu->setFlag(Flag::V, 0);
}
