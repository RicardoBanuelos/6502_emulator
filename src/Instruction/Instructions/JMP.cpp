#include "Instruction/Instructions/JMP.h"

JMP::JMP(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("JMP", icpu, addressingMode, cycles)
{
}

JMP::~JMP()
{
}

void JMP::run()
{
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);

    if(mAddressingMode == AddressingMode::Absolute)
    {
        mIcpu->setRegister(Register::PC, addressingData.address);
    }
    else 
    {
        mIcpu->setRegister(Register::PC, addressingData.data);
    }

}
