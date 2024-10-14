#include "Instruction/Instructions/DEC.h"

DEC::DEC(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("DEC", icpu, addressingMode, cycles)
{
}

DEC::~DEC()
{
}

void DEC::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    uint8_t fetched = addressingData.data;

    mIcpu->writeByte(addressingData.address, --fetched);

    mIcpu->setFlag(Flag::Z, fetched == 0);
    mIcpu->setFlag(Flag::N, fetched & 0x80);
}
