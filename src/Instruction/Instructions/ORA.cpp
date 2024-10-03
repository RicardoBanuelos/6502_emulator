#include "Instruction/Instructions/ORA.h"



ORA::ORA(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("ORA", icpu, addressingMode, cycles)
{
}

ORA::~ORA()
{
}

void ORA::run()
{
    auto [data, address] = mIcpu->addressing(mAddressingMode);
    uint8_t accumulator = mIcpu->getRegister(Register::A);

    accumulator |= data;

    mIcpu->setRegister(Register::A, accumulator);

    mIcpu->setFlag(Flag::Z, accumulator == 0);
    mIcpu->setFlag(Flag::N, accumulator & Flag::N);
}
