#include "SED.h"


SED::SED(std::shared_ptr<ICPU> icpu)
    :   Instruction("SED", icpu, AddressingMode::Implied, 2)
{
}

SED::~SED()
{
}

void SED::run()
{
    mIcpu->setFlag(Flag::D, true);
}
