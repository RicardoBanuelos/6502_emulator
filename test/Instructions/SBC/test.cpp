#include <gtest/gtest.h>
#include "Instruction/Instructions/SBC.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 1000;

void ASSERT_ALL(uint16_t a, uint16_t fetched, uint16_t expected)
{

    ASSERT_EQ(cpu->getRegister(Register::A), expected & 0x00FF);
    ASSERT_EQ(cpu->getFlag(Flag::C), static_cast<bool>(expected & 0xFF00));
    ASSERT_EQ(cpu->getFlag(Flag::V), static_cast<bool>((expected ^ a) & (expected ^ fetched) & 0x0080));
    ASSERT_EQ(cpu->getFlag(Flag::N), static_cast<bool>(expected & Flag::N));
}


TEST(instructions, sbc_test_immediate)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();


        uint16_t address = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, address);

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t fetched = cpu->readByte(address);
        uint16_t carry = cpu->getFlag(Flag::C);

        uint16_t expected = a - fetched - carry;
        
        SBC ins(cpu, AddressingMode::Immediate, 2);
        ins.run();

        ASSERT_ALL(a, fetched, expected);
    }
}

TEST(instructions, sbc_test_zeroPage)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t fetched = cpu->readByte(lookUpAddr);
        uint16_t carry = cpu->getFlag(Flag::C);

        uint16_t expected = a - fetched - carry;

        SBC ins(cpu, AddressingMode::ZeroPage, 2);
        ins.run();

        ASSERT_ALL(a, fetched, expected);
    }
}

TEST(instructions, sbc_test_zeroPageX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t fetched = cpu->readByte(lookUpAddr + cpu->getRegister(Register::X));
        uint16_t carry = cpu->getFlag(Flag::C);

        uint16_t expected = a - fetched - carry;

        SBC ins(cpu, AddressingMode::ZeroPageX, 2);
        ins.run();

        ASSERT_ALL(a, fetched, expected);
    }
}

TEST(instructions, sbc_test_absolute)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUpAddr = cpu->readWord(currentAddr);

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t fetched = cpu->readByte(lookUpAddr);
        uint16_t carry = cpu->getFlag(Flag::C);

        uint16_t expected = a - fetched - carry;

        SBC ins(cpu, AddressingMode::Absolute, 2);
        ins.run();

        ASSERT_ALL(a, fetched, expected);
    }
}


TEST(instructions, sbc_test_absoluteX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUpAddr = cpu->readWord(currentAddr);

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t fetched = cpu->readByte(lookUpAddr + cpu->getRegister(Register::X));
        uint16_t carry = cpu->getFlag(Flag::C);

        uint16_t expected = a - fetched - carry;

        SBC ins(cpu, AddressingMode::AbsoluteOffsetX, 2);
        ins.run();

        ASSERT_ALL(a, fetched, expected);
    }
}

TEST(instructions, sbc_test_indirectX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t indirect = cpu->readWord(lookUpAddr + cpu->getRegister(Register::X));
        uint16_t fetched = cpu->readByte(indirect);
        uint16_t carry = cpu->getFlag(Flag::C);

        uint16_t expected = a - fetched - carry;

        SBC ins(cpu, AddressingMode::IndirectX, 2);
        ins.run();

        ASSERT_ALL(a, fetched, expected);
    }
}

TEST(instructions, sbc_test_indirectY)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t indirect = cpu->readWord(lookUpAddr + cpu->getRegister(Register::Y));
        uint16_t fetched = cpu->readByte(indirect);
        uint16_t carry = cpu->getFlag(Flag::C);

        uint16_t expected = a - fetched - carry;

        SBC ins(cpu, AddressingMode::IndirectY, 2);
        ins.run();

        ASSERT_ALL(a, fetched, expected);
    }
}



TEST(instructions, sbc_test_absoluteY)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUpAddr = cpu->readWord(currentAddr);

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t fetched = cpu->readByte(lookUpAddr + cpu->getRegister(Register::Y));
        uint16_t carry = cpu->getFlag(Flag::C);

        uint16_t expected = a - fetched - carry;

        SBC ins(cpu, AddressingMode::AbsoluteOffsetY, 2);
        ins.run();

        ASSERT_ALL(a, fetched, expected);
    }
}

int main(int argc, char** argv)
{

    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));

    mem->initialize();
    mem->randomize();
    
    CPU *cppu = static_cast<CPU*>(cpu.get());
    cppu->init();
    cppu->reset();
    

    cppu->connectBus(bus);
    bus->connectMemory(mem);

    return RUN_ALL_TESTS();
}