#include "STY.h"

STY::STY(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("STY", icpu, addressingMode, cycles)
{
}

STY::~STY()
{
}

void STY::run()
{
    AddressingData AddressingData = mIcpu->addressing(mAddressingMode);
    mIcpu->writeByte(AddressingData.address, mIcpu->getRegister(Register::Y));
}
