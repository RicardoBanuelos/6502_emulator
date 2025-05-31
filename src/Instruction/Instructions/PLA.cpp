#include "PLA.h"


PLA::PLA(std::shared_ptr<ICPU> icpu, uint8_t cycles)
    :   Instruction("PLA", icpu, AddressingMode::Implied, cycles)   
{
}

PLA::~PLA()
{
}

void PLA::run()
{
    // Pull byte from stack and store in accumulator
    uint8_t value = mIcpu->popByte();
    mIcpu->setRegister(Register::A, value);

    // Update processor status flags
    mIcpu->setFlag(Flag::Z, value == 0);     // Set if accumulator is zero
    mIcpu->setFlag(Flag::N, (value & 0x80) != 0); // Set if bit 7 is set
}