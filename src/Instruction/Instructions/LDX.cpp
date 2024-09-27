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
    mIcpu->registers().X = mIcpu->addressing(mAddreesingMode).data;
    mIcpu->registers().status.setFlag(Flag::Z, mIcpu->registers().X == 0);
    mIcpu->registers().status.setFlag(Flag::N, mIcpu->registers().X & (1 << 7));
}
