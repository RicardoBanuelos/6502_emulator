#include <gtest/gtest.h>
#include "Instruction/Instructions/CPY.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

void ASSERT_ALL(uint16_t Y, uint16_t M, uint16_t result)
{
    ASSERT_EQ(cpu->getFlag(Flag::C), Y >= M);
    ASSERT_EQ(cpu->getFlag(Flag::Z), Y == M);
    ASSERT_EQ(cpu->getFlag(Flag::N), static_cast<bool>(result & (1 << 7)));
}

TEST(instructions, cpy_immediate)
{
    cpu->reset();
    cpu->randomizeRegisters();

    uint16_t address = cpu->getRegister(Register::PC);
    uint16_t Y = cpu->getRegister(Register::Y);
    uint16_t M = cpu->readByte(address);
    uint8_t result = Y - M;

    std::unique_ptr<CPY> instruction(new CPY(cpu, AddressingMode::Immediate, 2));
    instruction->run();

    ASSERT_ALL(Y, M, result);
}

TEST(instructions, cpy_zero_page)
{
    cpu->reset();
    cpu->randomizeRegisters();

    uint16_t address = cpu->getRegister(Register::PC);
    uint16_t zeroPageAddress = cpu->readByte(address);

    uint16_t Y = cpu->getRegister(Register::Y);
    uint16_t M = cpu->readByte(zeroPageAddress);
    uint8_t result = Y - M;

    std::unique_ptr<CPY> instruction(new CPY(cpu, AddressingMode::ZeroPage, 3));
    instruction->run();

    ASSERT_ALL(Y, M, result);
}

TEST(instructions, cpy_absolute)
{
    cpu->reset();
    cpu->randomizeRegisters();

    uint16_t address = cpu->getRegister(Register::PC);
    uint16_t absoluteAddress = cpu->readWord(address);

    uint16_t Y = cpu->getRegister(Register::Y);
    uint16_t M = cpu->readByte(absoluteAddress);
    uint8_t result = Y - M;

    std::unique_ptr<CPY> instruction(new CPY(cpu, AddressingMode::Absolute, 4));
    instruction->run();

    ASSERT_ALL(Y, M, result);
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