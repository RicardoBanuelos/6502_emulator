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
    AddressingData addressingData = mIcpu->addressing(mAddreesingMode);
    mIcpu->registers().A = addressingData.data;
    mIcpu->registers().statusRegister.setFlag(Flag::Z, mIcpu->registers().A == 0);
    mIcpu->registers().statusRegister.setFlag(Flag::N, mIcpu->registers().A & (1 << 7));
}
