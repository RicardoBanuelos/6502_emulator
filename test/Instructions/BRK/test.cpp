#include <gtest/gtest.h>
#include "Instruction/Instructions/BRK.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

TEST(instructions, brk_implied)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        uint16_t prevPC = cpu->getRegister(Register::PC);
        uint16_t prevFlags = cpu->getRegister(Register::PS);
        uint16_t newPC = cpu->readWord(0xFFFE);

        std::unique_ptr<BRK> instruction(new BRK(cpu, AddressingMode::Implied, 7));
        instruction->run();

        ASSERT_EQ(prevFlags, cpu->popByte());
        ASSERT_EQ(prevPC, cpu->popWord());
        ASSERT_EQ(newPC, cpu->getRegister(Register::PC));
        ASSERT_EQ(cpu->getFlag(Flag::B), 1);
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
   
    int result = RUN_ALL_TESTS();
    return result;
}