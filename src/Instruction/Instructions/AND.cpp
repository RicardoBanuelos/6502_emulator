#include "Instruction/Instructions/AND.h"
#include "AND.h"

AND::AND(std::shared_ptr<ICPU> icpu, std::function<uint16_t()> addressingFunction, uint8_t cycles)
    :   Instruction("AND", icpu, addressingFunction, cycles)
{}

AND::~AND()
{}

void AND::run()
{
    uint16_t fetched = mAddressingFunction();
    uint16_t result = mIcpu->registers().A & fetched;

    mIcpu->registers().status.Z = result & 0x00FF == 0 ? 1 : 0;
    mIcpu->registers().status.N = result & 0x80 ? 1 : 0; 

    mIcpu->registers().A = result & 0x00FF;
}
