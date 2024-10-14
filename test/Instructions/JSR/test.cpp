#include <gtest/gtest.h>
#include "Instruction/Instructions/JSR.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 10000;


TEST(instructions, jsr_test)
{

    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t addr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, addr);

        uint16_t pcAddressExpeced = cpu->readWord(addr);

        JSR jsr(cpu);
        jsr.run();

        uint16_t addrInStack = cpu->popWord();

        ASSERT_EQ(addrInStack, addr + 2);
        ASSERT_EQ(cpu->getRegister(Register::PC), pcAddressExpeced);
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