#include "Instruction/Instructions/CPX.h"

CPX::CPX(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("CPX", icpu, addressingMode, cycles)
{
}

CPX::~CPX()
{
}

void CPX::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);

    uint8_t X = mIcpu->getRegister(Register::X);
    uint8_t fetched = addressingData.data;
    uint8_t result = X - fetched;

    mIcpu->setFlag(Flag::C, X >= fetched);
    mIcpu->setFlag(Flag::Z, X == fetched);
    mIcpu->setFlag(Flag::N, result & 0x80);
}
