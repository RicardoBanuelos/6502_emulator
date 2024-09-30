#include "Instruction/Instructions/SBC.h"

SBC::SBC(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("SBC", icpu, addressingMode, cycles)
{
}

SBC::~SBC()
{
}

void SBC::run()
{
    uint16_t value = mIcpu->addressing(mAddreesingMode).data ^ 0x00FF;

    uint16_t tmp = static_cast<uint16_t>(mIcpu->registers().A) + value + static_cast<uint16_t>(mIcpu->registers().status.C);
    
    mIcpu->registers().status.setFlag(Flag::C, tmp & 0xFF00);
    mIcpu->registers().status.setFlag(Flag::Z, (tmp & 0x00FF) == 0);
    mIcpu->registers().status.setFlag(Flag::V, (tmp ^ static_cast<uint16_t>(mIcpu->registers().A) & (tmp ^ value) & 0x0080));
    mIcpu->registers().status.setFlag(Flag::N, (tmp & 0x0080));

    mIcpu->registers().A = tmp & 0x00FF;
}
