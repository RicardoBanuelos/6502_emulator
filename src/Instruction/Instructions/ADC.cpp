#include "Instruction/Instructions/ADC.h"
#include "ADC.h"

ADC::ADC(std::shared_ptr<ICPU> icpu, std::function<uint16_t()> addressingFunction, uint8_t cycles)
    :   Instruction("ADC", icpu, addressingFunction, cycles)
{
}

ADC::~ADC()
{
}

void ADC::run()
{
    uint16_t fetched = mAddressingFunction();
    uint16_t result = mIcpu->registers().A + fetched + mIcpu->registers().status.C;

    mIcpu->registers().status.C = result > 255 ? 1 : 0;
    mIcpu->registers().status.Z = result & 0x00FF == 0 ? 1 : 0;

    // Overflow formula based on javidx9
    // Explanation here: https://github.com/OneLoneCoder/olcNES/blob/master/Part%232%20-%20CPU/olc6502.cpp

    uint16_t a = mIcpu->registers().A;
    bool overflow = ~((a ^ fetched) & (a ^ result) & 0x0080);
    
    mIcpu->registers().status.V = overflow;
    mIcpu->registers().status.N = result & 0x80 ? 1 : 0;

    mIcpu->registers().A = result & 0x00FF;
}
