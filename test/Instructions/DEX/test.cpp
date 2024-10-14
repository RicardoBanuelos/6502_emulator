#include <gtest/gtest.h>
#include "Instruction/Instructions/DEX.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

TEST(instructions, dex_implied)
{
    for(int i = 0; i < 1000; ++i)
    {    
        cpu->reset();
        cpu->randomizeRegisters();

        uint8_t result = cpu->getRegister(Register::X) - 1;

        std::unique_ptr<DEX> instruction(new DEX(cpu, AddressingMode::Implied, 2));
        instruction->run();

        ASSERT_EQ(cpu->getFlag(Flag::Z), result == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), static_cast<bool>(result & (1 << 7)));
        ASSERT_EQ(cpu->getRegister(Register::X), result);
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