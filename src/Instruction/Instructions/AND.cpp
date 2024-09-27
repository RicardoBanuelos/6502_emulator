#include "Instruction/Instructions/AND.h"

AND::AND(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("AND", icpu, addressingMode, cycles)
{
}

AND::~AND()
{}

void AND::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    uint16_t fetched = addressingData.data;

    uint16_t result = mIcpu->registers().A & fetched;

    mIcpu->registers().status.Z = result & 0x00FF == 0;
    mIcpu->registers().status.N = result & 0x80;

    mIcpu->registers().A = result & 0x00FF;
}
