#include "BIT.h"

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

    mIcpu->setFlag(Flag::Z, result == 0);
    mIcpu->setFlag(Flag::V, (M & 0x40) != 0);  // Copy bit 6 from memory
    mIcpu->setFlag(Flag::N, (M & 0x80) != 0);  // Copy bit 7 from memory
}
