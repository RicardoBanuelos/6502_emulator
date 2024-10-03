#include <gtest/gtest.h>
#include "Instruction/Instructions/SEC.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 10000;


TEST(instructions, sec_test_implied)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        SEC ins(cpu);
        ins.run();

        ASSERT_EQ(true, cpu->getFlag(Flag::C));
    }
}

int main(int argc, char** argv)
{

    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));

    mem->initialize();
    mem->randomize();
    
    CPU *cppu = static_cast<CPU*>(cpu.get());
    cppu->init();
    cppu->reset();
    

    cppu->connectBus(bus);
    bus->connectMemory(mem);

    return RUN_ALL_TESTS();
}