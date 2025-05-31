#include "ROR.h"

ROR::ROR(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    : Instruction("ROR", icpu, addressingMode, cycles)
{
}

ROR::~ROR()
{
}

void ROR::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);

    // Fetch the data
    uint8_t data = addressingData.data;
    
    // Extract the bit 0 which will become the carry flag
    bool newCarry = (data & 0x01) != 0;
    
    // Shift right by 1
    uint8_t result = data >> 1;
    
    // If carry was set, set bit 7 in the result
    if (mIcpu->getFlag(Flag::C)) {
        result |= 0x80;
    }
    
    // Set flags
    mIcpu->setFlag(Flag::C, newCarry);         // Carry is the original bit 0
    mIcpu->setFlag(Flag::Z, result == 0);      // Zero flag if result is 0
    mIcpu->setFlag(Flag::N, (result & 0x80) != 0); // Negative flag if bit 7 is set
    
    // Write the result back
    if (mAddressingMode == AddressingMode::Accumulator) {
        mIcpu->setRegister(Register::A, result);
    } else {
        mIcpu->writeByte(addressingData.address, result);
    }
}