#include <gtest/gtest.h>
#include "Instruction/Instructions/RTI.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 1000;


TEST(instructions, rti_test_implied)
{
    const uint16_t stackPointerOffset = 0x0100;

    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t pcExpected =  rand() % UINT16_MAX;
        uint8_t psExpected =  rand() % UINT8_MAX;

        cpu->pushWord(pcExpected);
        cpu->pushByte(psExpected);
        
        RTI ins(cpu);
        ins.run();

        ASSERT_EQ(pcExpected, cpu->getRegister(Register::PC));
        ASSERT_EQ(psExpected, cpu->getRegister(Register::PS));

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