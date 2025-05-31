#include "Registers.h"
#include <iostream>


PPURegisters::PPURegisters()
    : reg(REGISTERS_SIZE, 0)
{
}

PPURegisters::~PPURegisters() = default;

uint8_t PPURegisters::read(uint16_t addr) const
{
    return reg[addr & REGISTERS_MASK];
}

void PPURegisters::write(uint16_t addr, uint8_t data)
{
    reg[addr & REGISTERS_MASK] = data;
}