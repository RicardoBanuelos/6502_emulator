#include "Instruction/Instruction.h"



Instruction::Instruction(const std::string &name, std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   mName(name),
        mIcpu(icpu),
        mAddreesingMode(addressingMode),
        mCycles(cycles)
{

}


Instruction::~Instruction()
{
}
