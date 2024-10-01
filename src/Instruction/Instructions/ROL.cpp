#include "Instruction/Instructions/ROL.h"

ROL::ROL(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("ROL", icpu, addressingMode, cycles)
{
}

ROL::~ROL()
{
}

void ROL::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    uint8_t carry;
    uint8_t result;


    if(AddressingMode::Accumulator == mAddressingMode)
    {
        uint8_t &A = mIcpu->registers().A;
        carry = A & (1 << 7);

        A = (A << 1) | (A >> 7);

        result = A;
    }
    else 
    {
        uint8_t data = addressingData.data;
        carry = data & (1 << 7);

        data = (data << 1) | (data >> 7);
        mIcpu->writeByte(addressingData.address, data);

        result = data;
    }

    mIcpu->registers().statusRegister.setFlag(Flag::C, carry);
    mIcpu->registers().statusRegister.setFlag(Flag::Z, result == 0);
    mIcpu->registers().statusRegister.setFlag(Flag::N, result & (1 << 7));
}
