#include <gtest/gtest.h>
#include "Instruction/Instructions/CLV.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

TEST(instructions, clv_implied)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->setFlag(Flag::V, 1);

        std::unique_ptr<CLV> instruction(new CLV(cpu));
        instruction->run();

        ASSERT_EQ(cpu->getFlag(Flag::V), 0);
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