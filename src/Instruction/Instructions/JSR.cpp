#include "Instruction/Instructions/JSR.h"



JSR::JSR(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("JSR", icpu, addressingMode, cycles)
{
}
JSR::~JSR()
{

}

void JSR::run()
{
    mIcpu->pushWord(mIcpu->addressing(mAddreesingMode));
}