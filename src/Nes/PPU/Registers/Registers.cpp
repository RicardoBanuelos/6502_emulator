#include "Registers.h"
#include <iostream>

PPURegisters::PPURegisters()
    : reg(REGISTERS_SIZE, 0)
{
}

PPURegisters::~PPURegisters() = default;

uint8_t PPURegisters::read(RegistersIndex regIndex) const
{
    if(regIndex >= REGISTERS_SIZE)
    {
        std::cerr << "Error: Invalid register index " << static_cast<int>(regIndex) << std::endl;
        throw std::out_of_range("Invalid register index");
    }
    return reg[regIndex];
}

void PPURegisters::write(RegistersIndex regIndex, uint8_t value)
{
    if(regIndex >= REGISTERS_SIZE)
    {
        std::cerr << "Error: Invalid register index " << static_cast<int>(regIndex) << std::endl;
        throw std::out_of_range("Invalid register index");
    }
    
    reg[regIndex] = value;
}



