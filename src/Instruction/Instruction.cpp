#include "Instruction/Instruction.h"



Instruction::Instruction(const std::string &name, std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   mName(name),
        mIcpu(icpu),
        mAddressingMode(addressingMode),
        mCycles(cycles)
{

}


Instruction::~Instruction()
{
}

AddressingMode Instruction::mode() const
{
    return mAddressingMode;
}

uint8_t Instruction::cycles() const
{
    return mCycles;
}

const std::string &Instruction::name() const
{
    return mName;
}
