#include "Instruction/Instruction.h"



Instruction::Instruction(const std::string &name, std::shared_ptr<ICPU> icpu, std::function<uint16_t()> addressingFunction, uint8_t cycles)
    :   mName(name),
        mIcpu(icpu),
        mAddressingFunction(addressingFunction),
        mCycles(cycles)
{

}


Instruction::~Instruction()
{
}
