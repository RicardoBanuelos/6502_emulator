#include <gtest/gtest.h>
#include "Instruction/Instructions/INC.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

void ASSERT_ALL(uint8_t M)
{
    ASSERT_EQ(cpu->getFlag(Flag::Z), M == 0);
    ASSERT_EQ(cpu->getFlag(Flag::N), static_cast<bool>(M & (1 << 7)));
}

TEST(instructions, inc_zero_page)
{
    for(int i = 0; i < 1000; ++i)
    {    
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t zeroPageAddress = cpu->readByte(address);

        uint8_t M = cpu->readByte(zeroPageAddress) + 1;

        std::unique_ptr<INC> instruction(new INC(cpu, AddressingMode::ZeroPage, 3));
        instruction->run();

        ASSERT_ALL(M);
        ASSERT_EQ(cpu->readByte(zeroPageAddress), M);
    }
}

TEST(instructions, inc_zero_page_x)
{
    for(int i = 0; i < 1000; ++i)
    {    
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t zeroPageAddress = cpu->readByte(address) + cpu->getRegister(Register::X);

        uint8_t M = cpu->readByte(zeroPageAddress) + 1;

        std::unique_ptr<INC> instruction(new INC(cpu, AddressingMode::ZeroPageX, 3));
        instruction->run();

        ASSERT_ALL(M);
        ASSERT_EQ(cpu->readByte(zeroPageAddress), M);
    }
}

TEST(instructions, inc_absolute)
{
    for(int i = 0; i < 1000; ++i)
    {    
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t absoluteAddress = cpu->readWord(address);

        uint8_t M = cpu->readByte(absoluteAddress) + 1;

        std::unique_ptr<INC> instruction(new INC(cpu, AddressingMode::Absolute, 4));
        instruction->run();

        ASSERT_ALL(M);
        ASSERT_EQ(cpu->readByte(absoluteAddress), M);
    }
}

TEST(instructions, inc_absolute_x)
{
    for(int i = 0; i < 1000; ++i)
    {    
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t absoluteAddress = cpu->readWord(address) + cpu->getRegister(Register::X);

        uint8_t M = cpu->readByte(absoluteAddress) + 1;

        std::unique_ptr<INC> instruction(new INC(cpu, AddressingMode::AbsoluteX, 4));
        instruction->run();

        ASSERT_ALL(M);
        ASSERT_EQ(cpu->readByte(absoluteAddress), M);
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