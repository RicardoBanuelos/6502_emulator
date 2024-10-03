#include "Instruction/Instructions/CLI.h"

CLI::CLI(std::shared_ptr<ICPU> icpu, AddressingMode addressingMode, uint8_t cycles)
    :   Instruction("CLI", icpu, addressingMode, cycles)
{
}

CLI::~CLI()
{
}

void CLI::run()
{
    mIcpu->setFlag(Flag::I, 0);
}
