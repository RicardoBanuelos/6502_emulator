#include "TSX.h"

TSX::TSX(std::shared_ptr<ICPU> icpu)
    :   Instruction("TSX", icpu, AddressingMode::Implied, 2)
{
}

TSX::~TSX()
{
}

void TSX::run()
{
    mIcpu->setRegister(Register::X, mIcpu->getRegister(Register::SP));
    
    uint8_t reg = mIcpu->getRegister(Register::X);

    mIcpu->setFlag(Flag::Z, reg == 0);
    mIcpu->setFlag(Flag::N, reg & Flag::N);
}
