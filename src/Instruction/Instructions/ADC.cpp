#include "Instruction/Instructions/ADC.h"

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
    
    uint16_t result = mIcpu->getRegister(Register::A) + fetched + mIcpu->getFlag(Flag::C);

    // Overflow formula based on javidx9
    // Explanation here: https://github.com/OneLoneCoder/olcNES/blob/master/Part%232%20-%20CPU/olc6502.cpp
    uint16_t a = mIcpu->getRegister(Register::A);
    bool overflow = ~((a ^ fetched) & (a ^ result) & 0x0080);
    mIcpu->setFlag(Flag::V, overflow);

    mIcpu->setFlag(Flag::C, result > 255);
    mIcpu->setFlag(Flag::Z, result & 0x00FF == 0);
    mIcpu->setFlag(Flag::N, result & 0x80);
    mIcpu->setRegister(Register::A, result & 0x00FF);
}
