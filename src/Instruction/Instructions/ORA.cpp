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
    AddressingData addressingData = mIcpu->addressing(mAddreesingMode);

    if(AddressingMode::Immediate == mAddreesingMode)
    {
        mIcpu->registers().A |= addressingData.address;
    }
    else 
    {
        mIcpu->registers().A |= addressingData.data;
    }

    mIcpu->registers().statusRegister.setFlag(Flag::Z, mIcpu->registers().A == 0);
    mIcpu->registers().statusRegister.setFlag(Flag::N, mIcpu->registers().A & (1 << 7));
}
