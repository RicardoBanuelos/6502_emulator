#include "Instruction/Instructions/LDY.h"

LDY::LDY(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("LDY", icpu, addressingMode, cycles)
{
}

LDY::~LDY()
{
}

void LDY::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddreesingMode);

    if(mAddreesingMode == AddressingMode::Immediate)
    {
        mIcpu->registers().Y = static_cast<uint8_t>(addressingData.address);
    }
    else 
    {
        mIcpu->registers().Y = addressingData.data;
    }

    mIcpu->registers().status.setFlag(Flag::Z, mIcpu->registers().Y == 0);
    mIcpu->registers().status.setFlag(Flag::N, mIcpu->registers().Y & (1 << 7));
}

