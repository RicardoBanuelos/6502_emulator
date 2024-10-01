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
    uint16_t result = mIcpu->getRegister(Register::A) & addressingData.data;

    mIcpu->setFlag(Flag::Z, result & 0x00FF == 0);
    mIcpu->setFlag(Flag::V, result & (1 << 6));
    mIcpu->setFlag(Flag::N, result & (1 << 7));
}
