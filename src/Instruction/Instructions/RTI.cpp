#include "Instruction/Instructions/RTI.h"


RTI::RTI(std::shared_ptr<ICPU> icpu)
    :   Instruction("RTI", icpu, AddressingMode::Implied, 6)
{
}

RTI::~RTI()
{
}

void RTI::run()
{
    mIcpu->registers().status.byte = mIcpu->popByte();
    mIcpu->registers().PC = mIcpu->popWord();
}
