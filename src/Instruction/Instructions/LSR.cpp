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
    const AddressingData addressingData = mIcpu->addressing(mAddreesingMode);
    uint8_t carry = 0;
    uint8_t result;

    if(mAddreesingMode == AddressingMode::Accumulator)
    {
        carry = mIcpu->registers().A & 0x01;
        result = mIcpu->registers().A >> 1;
        result = mIcpu->registers().A;
    }
    else 
    {
        carry = addressingData.data & 0x01;
        result = addressingData.data >> 1;

        mIcpu->writeByte(addressingData.address, result);
    }

    mIcpu->registers().status.setFlag(Flag::C, carry);
    mIcpu->registers().status.setFlag(Flag::Z, result == 0);
    mIcpu->registers().status.setFlag(Flag::N, result & (1 << 7));

}
