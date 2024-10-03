#include "SEI.h"

SEI::SEI(std::shared_ptr<ICPU> icpu)
    :   Instruction("SEI", icpu, AddressingMode::Implied, 2)
{
}

SEI::~SEI()
{
}

void SEI::run()
{
    mIcpu->setFlag(Flag::I, true);
}
