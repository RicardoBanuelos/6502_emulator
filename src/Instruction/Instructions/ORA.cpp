#include "Instruction/Instructions/ORA.h"



ORA::ORA(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("ORA", icpu, addressingMode, cycles)
{
}

ORA::~ORA()
{
}

void ORA::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);

    if(AddressingMode::Immediate == mAddressingMode)
    {
        mIcpu->registers().A |= addressingData.address;
    }
    else 
    {
        mIcpu->registers().A |= addressingData.data;
    }

    mIcpu->registers().status.setFlag(Flag::Z, mIcpu->registers().A == 0);
    mIcpu->registers().status.setFlag(Flag::N, mIcpu->registers().A & (1 << 7));
}
