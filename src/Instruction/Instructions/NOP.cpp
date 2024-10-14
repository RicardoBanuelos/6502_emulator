#include "Instruction/Instructions/NOP.h"


NOP::NOP(std::shared_ptr<ICPU> icpu)
    :   Instruction("NOP", icpu, AddressingMode::Implied, 2)
{
}
NOP::~NOP()
{
}
void NOP::run()
{
    // :)
}