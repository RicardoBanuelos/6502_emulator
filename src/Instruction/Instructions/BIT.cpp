#include "Instruction/Instructions/BIT.h"

BIT::BIT(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("BIT", icpu, addressingMode, cycles)
{

}

BIT::~BIT()
{
}

void BIT::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    uint16_t result = mIcpu->registers().A & addressingData.data;

    mIcpu->registers().status.Z = result & 0x00FF == 0;
    mIcpu->registers().status.N = result & (1 << 7);
    mIcpu->registers().status.Z = result & (1 << 6);
}
