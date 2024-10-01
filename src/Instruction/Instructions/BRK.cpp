#include "Instruction/Instructions/BRK.h"

BRK::BRK(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("BRK", icpu, addressingMode, cycles)
{
}

BRK::~BRK()
{
}

void BRK::run()
{
    
}
