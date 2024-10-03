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
        cpu->reset();
        cpu->setFlag(Flag::V, 0);
        uint16_t expected = cpu->getRegister(Register::PC) + 1;

        std::unique_ptr<BVS> instruction(new BVS(cpu, AddressingMode::Relative, 2));
        instruction->run();

        uint16_t newPC = cpu->getRegister(Register::PC);

        ASSERT_EQ(expected, newPC);

        uint16_t relativeAddress = cpu->readByte(cpu->getRegister(Register::PC));
        expected = cpu->getRegister(Register::PC) + relativeAddress + 1;
        cpu->setFlag(Flag::V, 1);

        instruction->run();
        newPC = cpu->getRegister(Register::PC);

        ASSERT_EQ(expected, newPC);
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