#include "Instruction/Instructions/SEC.h"


SEC::SEC(std::shared_ptr<ICPU> icpu)
    :   Instruction("SEC", icpu, AddressingMode::Implied, 2)
{

}


SEC::~SEC()
{

}


void SEC::run()
{
    mIcpu->setFlag(Flag::C, true);
}
