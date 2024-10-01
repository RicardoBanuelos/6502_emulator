#include "Instruction/Instructions/LDX.h"


LDX::LDX(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("LDX", icpu, addressingMode, cycles)
{
}

LDX::~LDX()
{
}

void LDX::run()
{
    // AddressingData addressingData = mIcpu->addressing(mAddressingMode);

    // if(mAddressingMode == AddressingMode::Immediate)
    // {
    //     mIcpu->registers().X = static_cast<uint8_t>(addressingData.address);
    // }
    // else 
    // {
    //     mIcpu->registers().X = addressingData.data;
    // }

    // mIcpu->registers().statusRegister.setFlag(Flag::Z, mIcpu->registers().X == 0);
    // mIcpu->registers().statusRegister.setFlag(Flag::N, mIcpu->registers().X & (1 << 7));
}
