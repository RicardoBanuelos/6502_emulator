#include "Instruction/Instructions/LDA.h"

LDA::LDA(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("LDA", icpu, addressingMode, cycles)
{
}

LDA::~LDA()
{
}

void LDA::run()
{
    mIcpu->registers().A = mIcpu->addressing(mAddreesingMode).data;
    mIcpu->registers().status.setFlag(Flag::Z, mIcpu->registers().A == 0);
    mIcpu->registers().status.setFlag(Flag::N, mIcpu->registers().A & (1 << 7));
}
