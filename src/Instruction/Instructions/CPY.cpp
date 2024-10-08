#include "Instruction/Instructions/CPY.h"

CPY::CPY(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("CPY", icpu, addressingMode, cycles)
{
}

CPY::~CPY()
{
}

void CPY::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);

    uint8_t Y = mIcpu->getRegister(Register::Y);
    uint8_t fetched = addressingData.data;
    uint8_t result = Y - fetched;

    mIcpu->setFlag(Flag::C, Y >= fetched);
    mIcpu->setFlag(Flag::Z, Y == fetched);
    mIcpu->setFlag(Flag::N, result & 0x80);
}
