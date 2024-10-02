#include <iostream>

#include "Bus/Bus.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"

int main(int, char**)
{
    std::shared_ptr<Memory> memory = std::make_shared<Memory>();
    memory->initialize();

    std::shared_ptr<Bus> bus = std::make_shared<Bus>();
    bus->connectMemory(memory);

    std::shared_ptr<CPU> cpu = std::make_shared<CPU>();
    cpu->init();
    cpu->connectBus(bus);
    cpu->reset();
    cpu->execute();
    
    return 0;
}
