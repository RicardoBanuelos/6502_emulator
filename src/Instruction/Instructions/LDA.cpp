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
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    mIcpu->setRegister(Register::A, addressingData.data);
    mIcpu->setFlag(Flag::Z, mIcpu->getRegister(Register::A) == 0);
    mIcpu->setFlag(Flag::N, mIcpu->getRegister(Register::A) & 0x80);

}
