#include <gtest/gtest.h>
#include "Instruction/Instructions/PLA.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 10000;


TEST(instructions, pla_test_implied)
{
    const uint16_t stackPointerOffset = 0x0100;

    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint8_t expected = rand() % UINT8_MAX;
        cpu->pushByte(expected);

        PLA ins(cpu);
        ins.run();

        ASSERT_EQ(expected, cpu->getRegister(Register::A));
        ASSERT_EQ(expected == 0, cpu->getFlag(Flag::Z));
        ASSERT_EQ((expected & Flag::N) > 0, cpu->getFlag(Flag::N));
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