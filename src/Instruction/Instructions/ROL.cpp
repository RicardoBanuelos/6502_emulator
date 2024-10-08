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

    uint16_t fetched = addressingData.data;
    uint16_t result = (fetched << 1) | static_cast<uint8_t>(mIcpu->getFlag(Flag::C));

    mIcpu->setFlag(Flag::C, result & 0xFF00);
    mIcpu->setFlag(Flag::Z, (result & 0x00FF) == 0);
    mIcpu->setFlag(Flag::N, (result & Flag::N));

    if(mAddressingMode == AddressingMode::Accumulator)
    {
        mIcpu->setRegister(Register::A, result);
    }
    else
    {
        mIcpu->writeByte(addressingData.address, result);
    }  
}
