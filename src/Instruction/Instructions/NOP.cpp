#include "Instruction/Instructions/NOP.h"


NOP::NOP(std::shared_ptr<ICPU> icpu, uint8_t cycles)
    :   Instruction("NOP", icpu, AddressingMode::NA, cycles)
{
}
NOP::~NOP()
{
}