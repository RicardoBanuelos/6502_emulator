#include "Instruction/Instructions/LDA.h"

LDA::LDA(std::shared_ptr<ICPU> icpu, std::function<uint16_t()> addressingFunction, uint8_t cycles)
    :   Instruction("LDA", icpu, addressingFunction, cycles)
{
}

LDA::~LDA()
{
}

void LDA::run()
{
    mIcpu->registers().A = mAddressingFunction();
}