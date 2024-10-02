#include "Instruction/Instructions/CLD.h"
#include "CLD.h"

CLD::CLD(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("CLD", icpu, addressingMode, cycles)
{
}

CLD::~CLD()
{
}

void CLD::run()
{
    mIcpu->setFlag(Flag::D, 0);
}
