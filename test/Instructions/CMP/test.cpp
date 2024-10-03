#include <gtest/gtest.h>
#include "Instruction/Instructions/CMP.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

void ASSERT_ALL(uint16_t A, uint16_t M, uint16_t result)
{
    ASSERT_EQ(cpu->getFlag(Flag::C), A >= M);
    ASSERT_EQ(cpu->getFlag(Flag::Z), A == M);
    ASSERT_EQ(cpu->getFlag(Flag::N), static_cast<bool>(result & (1 << 7)));
}

TEST(instructions, cmp_immediate)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t A = cpu->getRegister(Register::A);
        uint16_t M = cpu->readByte(address);
        uint8_t result = A - M;

        std::unique_ptr<CMP> instruction(new CMP(cpu, AddressingMode::Immediate, 2));
        instruction->run();

        ASSERT_ALL(A, M, result);
    }
}

TEST(instructions, cmp_zero_page)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t zeroPageAddress = cpu->readByte(address);

        uint16_t A = cpu->getRegister(Register::A);
        uint16_t M = cpu->readByte(zeroPageAddress);
        uint8_t result = A - M;

        std::unique_ptr<CMP> instruction(new CMP(cpu, AddressingMode::ZeroPage, 3));
        instruction->run();

        ASSERT_ALL(A, M, result);
    }
}

TEST(instructions, cmp_zero_page_x)
{
    for(int i = 0; i < 1000; ++i)
    {    
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t zeroPageAddress = cpu->readByte(address) + cpu->getRegister(Register::X);

        uint16_t A = cpu->getRegister(Register::A);
        uint16_t M = cpu->readByte(zeroPageAddress);
        uint8_t result = A - M;

        std::unique_ptr<CMP> instruction(new CMP(cpu, AddressingMode::ZeroPageX, 4));
        instruction->run();

        ASSERT_ALL(A, M, result);
    }
}

TEST(instructions, cmp_absolute)
{
    for(int i = 0; i < 1000; ++i)
    {    
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t absoluteAddress = cpu->readWord(address);

        uint16_t A = cpu->getRegister(Register::A);
        uint16_t M = cpu->readByte(absoluteAddress);
        uint8_t result = A - M;

        std::unique_ptr<CMP> instruction(new CMP(cpu, AddressingMode::Absolute, 4));
        instruction->run();

        ASSERT_ALL(A, M, result);
    }
}

TEST(instructions, cmp_absolute_x)
{
    for(int i = 0; i < 1000; ++i)
    {    
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t absoluteAddress = cpu->readWord(address) + cpu->getRegister(Register::X);

        uint16_t A = cpu->getRegister(Register::A);
        uint16_t M = cpu->readByte(absoluteAddress);
        uint8_t result = A - M;

        std::unique_ptr<CMP> instruction(new CMP(cpu, AddressingMode::AbsoluteOffsetX, 4));
        instruction->run();

        ASSERT_ALL(A, M, result);
    }
}

TEST(instructions, cmp_absolute_y)
{
    for(int i = 0; i < 1000; ++i)
    {    
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t absoluteAddress = cpu->readWord(address) + cpu->getRegister(Register::Y);

        uint16_t A = cpu->getRegister(Register::A);
        uint16_t M = cpu->readByte(absoluteAddress);
        uint8_t result = A - M;

        std::unique_ptr<CMP> instruction(new CMP(cpu, AddressingMode::AbsoluteOffsetY, 4));
        instruction->run();

        ASSERT_ALL(A, M, result);
    }
}

TEST(instructions, cmp_indirect_x)
{
    for(int i = 0; i < 1000; ++i)
    {    
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t PC = cpu->getRegister(Register::PC);
        uint16_t baseAddress = cpu->readByte(PC) + cpu->getRegister(Register::X);
        uint16_t indirectAddress = cpu->readWord(baseAddress);;

        uint16_t A = cpu->getRegister(Register::A);
        uint16_t M = cpu->readByte(indirectAddress);
        uint8_t result = A - M;

        std::unique_ptr<CMP> instruction(new CMP(cpu, AddressingMode::IndirectX, 6));
        instruction->run();

        ASSERT_ALL(A, M, result);
    }
}

TEST(instructions, cmp_indirect_y)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();

        uint16_t PC = cpu->getRegister(Register::PC);
        uint16_t baseAddress = cpu->readByte(PC) + cpu->getRegister(Register::Y);
        uint16_t indirectAddress = cpu->readWord(baseAddress);

        uint16_t expected = cpu->readByte(indirectAddress) & cpu->getRegister(Register::A);

        uint16_t A = cpu->getRegister(Register::A);
        uint16_t M = cpu->readByte(indirectAddress);
        uint8_t result = A - M;

        std::unique_ptr<CMP> instruction(new CMP(cpu, AddressingMode::IndirectY, 5));
        instruction->run();

        ASSERT_ALL(A, M, result);
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