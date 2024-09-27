#include "Instruction/Instructions/ADC.h"
#include "ADC.h"

ADC::ADC(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("ADC", icpu, addressingMode, cycles)
{
}

ADC::~ADC()
{
}

void ADC::run()
{
    uint16_t fetched = mIcpu->addressing(mAddressingMode).data;
    uint16_t result = mIcpu->registers().A + fetched + mIcpu->registers().status.C;

    mIcpu->registers().status.C = result > 255;
    mIcpu->registers().status.Z = result & 0x00FF == 0;

    // Overflow formula based on javidx9
    // Explanation here: https://github.com/OneLoneCoder/olcNES/blob/master/Part%232%20-%20CPU/olc6502.cpp

    uint16_t a = mIcpu->registers().A;
    bool overflow = ~((a ^ fetched) & (a ^ result) & 0x0080);
    
    mIcpu->registers().status.V = overflow;
    mIcpu->registers().status.N = result & 0x80;

    mIcpu->registers().A = result & 0x00FF;
}
