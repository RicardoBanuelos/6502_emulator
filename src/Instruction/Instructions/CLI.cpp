#include "Instruction/Instructions/CLI.h"

CLI::CLI(std::shared_ptr<ICPU> icpu)
    :   Instruction("CLI", icpu, AddressingMode::Implied, 2)
{
}

CLI::~CLI()
{
}

void CLI::run()
{
    mIcpu->setFlag(Flag::I, 0);
}
