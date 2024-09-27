#include "Instruction/Instructions/ASL.h"

ASL::ASL(std::shared_ptr<ICPU> icpu, std::function<uint16_t()> addressingFunction, uint8_t cycles)
    :   Instruction("ASL", icpu, addressingFunction, cycles)
{
}

ASL::~ASL()
{
}

void ASL::run()
{
    uint16_t fetched = mAddressingFunction();
}
