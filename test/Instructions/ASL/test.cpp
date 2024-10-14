#include <gtest/gtest.h>
#include "Instruction/Instructions/ASL.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

void ASSERT_ALL(uint16_t expected, uint8_t result)
{
    ASSERT_EQ(expected & 0x00FF, result);
    ASSERT_EQ(cpu->getFlag(Flag::C), (expected & 0xFF00) > 0);
    ASSERT_EQ(cpu->getFlag(Flag::Z), (expected & 0x00FF) == 0);
    ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
}

TEST(instructions, asl_accumulator)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t expected = cpu->getRegister(Register::A) << 1;    

        std::unique_ptr<ASL> instruction(new ASL(cpu, AddressingMode::Implied, 2));
        instruction->run();

        ASSERT_ALL(expected, cpu->getRegister(Register::A));
    }
}

TEST(instructions, asl_zero_page)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t zeroPageAddress = cpu->readByte(address);

        uint16_t expected = cpu->readByte(zeroPageAddress) << 1;

        std::unique_ptr<ASL> instruction(new ASL(cpu, AddressingMode::ZeroPage, 5));
        instruction->run();

        ASSERT_ALL(expected,  cpu->readByte(zeroPageAddress));
    }
}

TEST(instructions, asl_zero_page_x)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t zeroPageAddress = cpu->readByte(address) 
                                + cpu->getRegister(Register::X);

        uint16_t expected = cpu->readByte(zeroPageAddress) << 1;

        std::unique_ptr<ASL> instruction(new ASL(cpu, AddressingMode::ZeroPageX, 6));
        instruction->run();

        ASSERT_ALL(expected, cpu->readByte(zeroPageAddress));
    }
}

TEST(instructions, asl_absolute)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t absoluteAddress = cpu->readWord(address);

        uint16_t expected = cpu->readByte(absoluteAddress) << 1;

        std::unique_ptr<ASL> instruction(new ASL(cpu, AddressingMode::Absolute, 6));
        instruction->run();

        ASSERT_ALL(expected, cpu->readByte(absoluteAddress));   
    }
}

TEST(instructions, asl_absolute_x)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t absoluteAddress = cpu->readWord(address)
                                + cpu->getRegister(Register::X);;

        uint16_t expected = cpu->readByte(absoluteAddress) << 1;

        std::unique_ptr<ASL> instruction(new ASL(cpu, AddressingMode::AbsoluteX, 7));
        instruction->run();

        ASSERT_ALL(expected, cpu->readByte(absoluteAddress));;   
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