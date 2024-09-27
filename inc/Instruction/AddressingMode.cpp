#include "AddressingMode.h"

AddressingMode::AddressingMode(std::shared_ptr<ICPU> icpu)
    :   mIcpu(icpu)
{
}

AddressingMode::~AddressingMode()
{
}

uint8_t AddressingMode::Implied()
{
    return 0;
}

uint8_t AddressingMode::Immediate()
{
    return mIcpu->fetchByte();
}
