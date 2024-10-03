#include "Instruction/Instructions/ROR.h"

ROR::ROR(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("ROR", icpu, addressingMode, cycles)
{
}

ROR::~ROR()
{
}

void ROR::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);

    uint8_t data = addressingData.data;
    uint8_t carry = data & (1 >> 7);
    data = (data >> 1) | (data << 7);

    if(AddressingMode::Accumulator == mAddressingMode)
    {
        mIcpu->setRegister(Register::A, data);
    }
    else 
    {        
        mIcpu->writeByte(addressingData.address, data);
    }

    mIcpu->setFlag(Flag::C, carry);
    mIcpu->setFlag(Flag::Z, data == 0);
    mIcpu->setFlag(Flag::N, data & Flag::N);

}
