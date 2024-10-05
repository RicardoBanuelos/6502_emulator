#include <gtest/gtest.h>
#include "Instruction/Instructions/BVS.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

TEST(instructions, bvs_relative)
{
    for(int i = 0; i < 1000; ++i)
    {    
        cpu->randomizeFlags();
        cpu->setFlag(Flag::V, 1);

        uint16_t addr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, addr);


        uint8_t offset = cpu->readByte(addr);
        uint16_t expected = (cpu->getRegister(Register::PC) + 1) + static_cast<int8_t>(offset);
        
        std::unique_ptr<BVS> instruction(new BVS(cpu));
        instruction->run();


        ASSERT_EQ(expected, cpu->getRegister(Register::PC));  
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