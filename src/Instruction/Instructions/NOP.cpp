#include "Instruction/Instructions/NOP.h"


NOP::NOP(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("NOP", icpu, addressingMode, cycles)
{
}
NOP::~NOP()
{
}