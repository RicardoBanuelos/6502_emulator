#include "Instruction/Instructions/INC.h"

INC::INC(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("INC", icpu, addressingMode, cycles)
{
}

INC::~INC()
{
}

void INC::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    uint8_t result = addressingData.data + 1;

    mIcpu->writeByte(addressingData.address, result);
    mIcpu->setFlag(Flag::Z, result == 0);
    mIcpu->setFlag(Flag::N, result & (1 << 7));
}
