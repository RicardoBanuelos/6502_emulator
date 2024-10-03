#include "Instruction/Instructions/LSR.h"



LSR::LSR(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("LSR", icpu, addressingMode, cycles)
{
}

LSR::~LSR()
{
}

void LSR::run()
{
    const AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    uint8_t carry = 0;
    uint8_t result;

    if(mAddressingMode == AddressingMode::Accumulator)
    {
        carry = mIcpu->getRegister(Register::A) & 0x01;
        result = mIcpu->getRegister(Register::A) >> 1;
        mIcpu->setRegister(Register::A, result);
    }
    else 
    {
        carry = addressingData.data & 0x01;
        result = addressingData.data >> 1;
        mIcpu->writeByte(addressingData.address, result);
    }

    mIcpu->setFlag(Flag::C, carry);
    mIcpu->setFlag(Flag::Z, result == 0);
    mIcpu->setFlag(Flag::N, result & Flag::N);
}
