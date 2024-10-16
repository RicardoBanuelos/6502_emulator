#include <gtest/gtest.h>
#include "Instruction/Instructions/BIT.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

void ASSERT_ALL(uint16_t result, uint16_t fetched)
{
    ASSERT_EQ(cpu->getFlag(Flag::Z), result & 0x00FF == 0x00);
    ASSERT_EQ(cpu->getFlag(Flag::V), (bool)(fetched & (1 << 6)));
    ASSERT_EQ(cpu->getFlag(Flag::N), (bool)(fetched & (1 << 7)));
}

TEST(instructions, bit_zero_page)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t zeroPageAddress = cpu->readByte(cpu->getRegister(Register::PC));

        uint16_t M = cpu->readByte(zeroPageAddress);
        uint16_t A = cpu->getRegister(Register::A);
        uint16_t result = A & M;

        std::unique_ptr<BIT> instruction(new BIT(cpu, AddressingMode::ZeroPage, 3));
        instruction->run();

        ASSERT_ALL(result, M);
    }
}

TEST(instructions, bit_absolute)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t absoluteAddress = cpu->readWord(cpu->getRegister(Register::PC));

        uint16_t M = cpu->readByte(absoluteAddress);
        uint16_t A = cpu->getRegister(Register::A);
        uint16_t result = A & M;

        std::unique_ptr<BIT> instruction(new BIT(cpu, AddressingMode::Absolute, 4));
        instruction->run();

        ASSERT_ALL(result, M);   
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