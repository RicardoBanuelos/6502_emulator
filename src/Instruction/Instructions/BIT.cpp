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
    uint16_t M = addressingData.data;
    uint16_t A = mIcpu->getRegister(Register::A);

    uint16_t result = A & M;

    mIcpu->setFlag(Flag::Z, result & 0x00FF == 0x00);
    mIcpu->setFlag(Flag::V, M & (1 << 6));
    mIcpu->setFlag(Flag::N, M & (1 << 7));
}
