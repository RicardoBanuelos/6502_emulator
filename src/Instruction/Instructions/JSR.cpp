#include "Instruction/Instructions/JSR.h"

JSR::JSR(std::shared_ptr<ICPU> icpu, std::function<uint16_t()> addressingFunction, uint8_t cycles)
    :   Instruction("JSR", icpu, addressingFunction, cycles)
{
}
JSR::~JSR()
{
}
void JSR::run()
{
    mIcpu->pushWord(mIcpu->registers().PC + 2);
    mIcpu->registers().PC = mAddressingFunction();
}