#include "Instruction/Instructions/CMP.h"

CMP::CMP(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("CMP", icpu, addressingMode, cycles)
{
}

CMP::~CMP()
{
}

void CMP::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);

    uint8_t A = mIcpu->getRegister(Register::A);
    uint8_t fetched = addressingData.data;
    uint8_t result = A - fetched;

    mIcpu->setFlag(Flag::C, A >= fetched);
    mIcpu->setFlag(Flag::Z, A == fetched);
    mIcpu->setFlag(Flag::N, result & 0x80);
}
