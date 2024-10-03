#include <gtest/gtest.h>
#include "Instruction/Instructions/CLI.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

TEST(instructions, cli_implied)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->setFlag(Flag::I, 1);

        std::unique_ptr<CLI> instruction(new CLI(cpu, AddressingMode::Implied, 2));
        instruction->run();

        ASSERT_EQ(cpu->getFlag(Flag::I), 0);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    mem->initialize();
    mem->randomize();

    CPU *cppu = static_cast<CPU*>(cpu.get());
    cppu->init();

    cppu->connectBus(bus);
    bus->connectMemory(mem);
   
    return RUN_ALL_TESTS();
}