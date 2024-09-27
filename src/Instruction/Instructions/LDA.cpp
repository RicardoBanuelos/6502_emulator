#include "Instruction/Instructions/LDA.h"




LDA::LDA(const std::string &name, std::shared_ptr<ICPU> icpu, std::function<uint16_t()> addressingFunction, uint8_t cycles)
    :   Instruction(name, icpu, addressingFunction, cycles)
{
}

LDA::~LDA()
{
}

void LDA::run()
{
    mIcpu->registers().X = mAddressingFunction();
    //flags
}