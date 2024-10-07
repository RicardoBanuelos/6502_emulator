#include "Instruction/Instructions/ADC.h"
#include <iostream>

ADC::ADC(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("ADC", icpu, addressingMode, cycles)
{
}

ADC::~ADC()
{
}

void ADC::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    uint8_t address = addressingData.address;
    uint16_t fetched = addressingData.data;
    uint16_t a = mIcpu->getRegister(Register::A);
    uint16_t result = a + fetched + static_cast<uint16_t>(mIcpu->getFlag(Flag::C));

    // Overflow formula based on javidx9
    // Explanation here: https://github.com/OneLoneCoder/olcNES/blob/master/Part%232%20-%20CPU/olc6502.cpp
    bool overflow = (~(a ^ fetched) & (a ^ result)) & 0x0080;
    mIcpu->setFlag(Flag::V, overflow);

    mIcpu->setFlag(Flag::C, result > 255);
    mIcpu->setFlag(Flag::Z, (result & 0x00FF) == 0);
    mIcpu->setFlag(Flag::N, result & Flag::N);
    mIcpu->setRegister(Register::A, result & 0x00FF);
}
