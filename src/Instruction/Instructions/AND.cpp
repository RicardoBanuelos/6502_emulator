#include "Instruction/Instructions/AND.h"

AND::AND(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("AND", icpu, addressingMode, cycles)
{
}

AND::~AND()
{}

void AND::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    uint16_t M = addressingData.data;
    uint16_t A = mIcpu->getRegister(Register::A);

    uint16_t result = A & M;

    mIcpu->setFlag(Flag::Z, result & 0x00FF == 0);
    mIcpu->setFlag(Flag::N, result & 0x80);
    mIcpu->setRegister(Register::A, result & 0x00FF);
}
