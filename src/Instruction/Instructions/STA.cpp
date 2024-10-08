#include "STA.h"

STA::STA(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("STA", icpu, addressingMode, cycles)
{
}

STA::~STA()
{
}

void STA::run()
{
    AddressingData AddressingData = mIcpu->addressing(mAddressingMode);
    mIcpu->writeByte(AddressingData.address, mIcpu->getRegister(Register::A));
}
