#include <gtest/gtest.h>
#include "Instruction/Instructions/DEC.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

void ASSERT_ALL(uint16_t M)
{
    ASSERT_EQ(cpu->getFlag(Flag::C), M & 0x00FF == 0);
    ASSERT_EQ(cpu->getFlag(Flag::N), static_cast<bool>(M & (1 << 7)));
}

TEST(instructions, dec_zero_page)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t zeroPageAddress = cpu->readByte(address);

        uint16_t M = cpu->readByte(zeroPageAddress) - 1;

        std::unique_ptr<DEC> instruction(new DEC(cpu, AddressingMode::ZeroPage, 3));
        instruction->run();

        ASSERT_ALL(M);
        ASSERT_EQ(cpu->readByte(zeroPageAddress), M & 0x00FF);
    }
}

TEST(instructions, dec_zero_page_x)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t zeroPageAddress = cpu->readByte(address) + cpu->getRegister(Register::X);

        uint16_t M = cpu->readByte(zeroPageAddress) - 1;

        std::unique_ptr<DEC> instruction(new DEC(cpu, AddressingMode::ZeroPageX, 3));
        instruction->run();

        ASSERT_ALL(M);
        ASSERT_EQ(cpu->readByte(zeroPageAddress), M & 0x00FF);
    }
}

TEST(instructions, dec_absolute)
{
    for(int i = 0; i < 1000; ++i)
    {    
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t absoluteAddress = cpu->readWord(address);

        uint16_t M = cpu->readByte(absoluteAddress) - 1;

        std::unique_ptr<DEC> instruction(new DEC(cpu, AddressingMode::Absolute, 4));
        instruction->run();

        ASSERT_ALL(M);
        ASSERT_EQ(cpu->readByte(absoluteAddress), M & 0x00FF);
    }
}

TEST(instructions, dec_absolute_x)
{
    for(int i = 0; i < 1000; ++i)
    {    
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t absoluteAddress = cpu->readWord(address) + cpu->getRegister(Register::X);

        uint16_t M = cpu->readByte(absoluteAddress) - 1;

        std::unique_ptr<DEC> instruction(new DEC(cpu, AddressingMode::AbsoluteOffsetX, 4));
        instruction->run();

        ASSERT_ALL(M);
        ASSERT_EQ(cpu->readByte(absoluteAddress), M & 0x00FF);
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