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

    uint16_t fetched = addressingData.data;
    uint16_t result = static_cast<uint16_t>(mIcpu->getFlag(Flag::C) << 7) | (fetched >> 1);

    mIcpu->setFlag(Flag::C, result & 0x01);
    mIcpu->setFlag(Flag::Z, (result & 0x00FF) == 0);
    mIcpu->setFlag(Flag::N, (result & Flag::N));

    if(mAddressingMode == AddressingMode::Implied)
    {
        mIcpu->setRegister(Register::A, result);
    }
    else
    {
        mIcpu->writeByte(addressingData.address, result);
    }  

}
