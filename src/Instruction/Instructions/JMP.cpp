#include "JMP.h"

JMP::JMP(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("JMP", icpu, addressingMode, cycles)
{
}

JMP::~JMP()
{
}

void JMP::run()
{
    if (mAddressingMode == AddressingMode::Indirect) {
        // Handle the JMP indirect bug here, bypassing the normal addressing mechanism
        uint16_t addressPointer = mIcpu->fetchWord();
        uint16_t targetAddress;
        
        // Check if pointer is at the end of a page
        if ((addressPointer & 0xFF) == 0xFF) {
            // Page boundary bug implementation:
            // Read LSB from correct address
            uint8_t lsb = mIcpu->readByte(addressPointer);
            // Read MSB from same page (wrap around)
            uint8_t msb = mIcpu->readByte((addressPointer & 0xFF00));
            targetAddress = (msb << 8) | lsb;
        } else {
            // Normal case - read full word
            targetAddress = mIcpu->readWord(addressPointer);
        }
        
        // Jump to the target address
        mIcpu->setRegister(Register::PC, targetAddress);
    }
    else {
        // Handle Absolute mode normally
        AddressingData addressingData = mIcpu->addressing(mAddressingMode);
        mIcpu->setRegister(Register::PC, addressingData.address);
    }
}
