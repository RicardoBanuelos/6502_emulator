#include "STX.h"

STX::STX(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("STX", icpu, addressingMode, cycles)
{
}

STX::~STX()
{
}

void STX::run()
{
    AddressingData AddressingData = mIcpu->addressing(mAddressingMode);
    mIcpu->writeByte(AddressingData.address, mIcpu->getRegister(Register::X));
}
