#include "Instruction/Instructions/BNE.h"

BNE::BNE(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("BNE", icpu, addressingMode, cycles)
{
}

BNE::~BNE()
{
}

void BNE::run()
{
    if(!mIcpu->getFlag(Flag::Z))
        return;
        
    AddressingData addressingData = mIcpu->addressing(mAddressingMode);
    uint16_t pc = mIcpu->getRegister(Register::PC);
    mIcpu->setRegister(Register::PC, pc + addressingData.address);
}
