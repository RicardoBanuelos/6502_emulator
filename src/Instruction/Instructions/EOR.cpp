#include "Instruction/Instructions/EOR.h"

EOR::EOR(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("EOR", icpu, addressingMode, cycles)
{
}

EOR::~EOR()
{
}

void EOR::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);

    uint8_t A = mIcpu->getRegister(Register::A);
    uint8_t fetched = addressingData.data;
    uint8_t result = A ^ fetched;

    mIcpu->setRegister(Register::A, result);
    mIcpu->setFlag(Flag::Z, result == 0);
    mIcpu->setFlag(Flag::N, result & 0x80);
}
