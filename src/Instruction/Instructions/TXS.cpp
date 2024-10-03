#include "TXS.h"

TXS::TXS(std::shared_ptr<ICPU> icpu)
    :   Instruction("TXS", icpu, AddressingMode::Implied, 2)
{
}

TXS::~TXS()
{
}

void TXS::run()
{
    mIcpu->setRegister(Register::SP, mIcpu->getRegister(Register::X));
}
