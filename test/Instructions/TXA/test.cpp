#include <gtest/gtest.h>
#include "Instruction/Instructions/TXA.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 10000;


TEST(instructions, txa_test_implied)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        TXA ins(cpu);
        ins.run();


        ASSERT_EQ(cpu->getRegister(Register::A), cpu->getRegister(Register::X));
        ASSERT_EQ(cpu->getFlag(Flag::Z), cpu->getRegister(Register::A) == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), static_cast<bool>(cpu->getRegister(Register::A) & Flag::N));
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