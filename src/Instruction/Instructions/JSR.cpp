#include "Instruction/Instructions/JSR.h"



JSR::JSR(std::shared_ptr<ICPU> icpu)
    :   Instruction("JSR", icpu, AddressingMode::Absolute, 6)
{
}
JSR::~JSR()
{

}

void JSR::run()
{
    AddressingData addessingata = mIcpu->addressing(mAddressingMode);
    mIcpu->pushWord(mIcpu->getRegister(Register::PC));

    mIcpu->setRegister(Register::PC, addessingata.address);
}