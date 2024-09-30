#include <iostream>

#include "CPU/CPU.h"
#include "Instruction/Instructions/LDA.h"
int main(int, char**)
{
    std::shared_ptr<CPU> cpu = std::make_shared<CPU>();
    cpu->init();
    cpu->reset();
    cpu->execute();

    LDA *ins = new LDA(cpu, AddressingMode::Immediate, 2);
    ins->run();

    
    return 0;
}
